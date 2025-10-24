from pyspark.sql import SparkSession, functions as f
import time
import os


def get_file_size_mb(file_path):
    if os.path.isfile(file_path):
        return os.path.getsize(file_path) / (1024 * 1024)
    elif os.path.isdir(file_path):
        total_size = 0
        for dirpath, dirnames, filenames in os.walk(file_path):
            for filename in filenames:
                filepath = os.path.join(dirpath, filename)
                if os.path.isfile(filepath):
                    total_size += os.path.getsize(filepath)
        return total_size / (1024 * 1024)


def measure_read_time(spark, file_path, format_type):
    spark.catalog.clearCache()
    
    start_time = time.time()
    
    if format_type == "parquet":
        df = spark.read.parquet(file_path)
    elif format_type == "orc":
        df = spark.read.orc(file_path)
    else:
        df = spark.read.csv(file_path, header=True, inferSchema=True)
    
    end_time = time.time()
    
    return end_time - start_time, df.count()


def apply_transformation(df):
    length_sum_expr = f.lit(0)
    for col_name in df.columns:
        length_sum_expr = length_sum_expr + f.length(f.col(col_name).cast("string"))
    
    return df.withColumn("sum_lens", length_sum_expr)


def process_dataset(spark, input_file, dataset_name):
    start_time = time.time()
    df = spark.read.csv(input_file, header=True, inferSchema=True)
    read_time = time.time() - start_time
    
    df_transformed = apply_transformation(df)
    
    os.makedirs(f"results/{dataset_name}", exist_ok=True)
    
    results = {
        "dataset": dataset_name,
        "original_file": input_file,
        "original_size_mb": get_file_size_mb(input_file),
        "original_read_time": read_time
    }
    
    parquet_path = f"results/{dataset_name}/data.parquet"
    df_transformed.coalesce(1).write.mode("overwrite").parquet(parquet_path)
    parquet_size = get_file_size_mb(parquet_path)
    
    parquet_read_time, parquet_count = measure_read_time(spark, parquet_path, "parquet")
    
    results["parquet"] = {
        "size_mb": parquet_size,
        "read_time": parquet_read_time,
        "record_count": parquet_count
    }
    
    orc_path = f"results/{dataset_name}/data.orc"
    df_transformed.coalesce(1).write.mode("overwrite").orc(orc_path)
    orc_size = get_file_size_mb(orc_path)
    
    orc_read_time, orc_count = measure_read_time(spark, orc_path, "orc")
    
    results["orc"] = {
        "size_mb": orc_size,
        "read_time": orc_read_time,
        "record_count": orc_count
    }
    
    return results


def save_results(all_results):
    os.makedirs("results", exist_ok=True)
    
    with open("results/benchmark_results.csv", "w") as f:
        f.write("Dataset,Original_Size_MB,Parquet_Size_MB,ORC_Size_MB,CSV_Read_Time,Parquet_Read_Time,ORC_Read_Time\n")
        for result in all_results:
            dataset_name = result['dataset'].replace('_', '\\\\\\_')
            f.write(f"{dataset_name},{result['original_size_mb']:.2f},{result['parquet']['size_mb']:.2f},{result['orc']['size_mb']:.2f},{result['original_read_time']:.2f},{result['parquet']['read_time']:.2f},{result['orc']['read_time']:.2f}\n")


if __name__ == "__main__":
    spark = SparkSession.builder \
        .appName("DataFormatComparison") \
        .config("spark.sql.adaptive.enabled", "true") \
        .config("spark.sql.adaptive.coalescePartitions.enabled", "true") \
        .getOrCreate()
    
    datasets = [
        ("data/trade_data.csv", "trade_data"),
        ("data/market_orders.csv", "market_orders"),
        ("data/tweets.csv", "tweets")
    ]
    
    all_results = []
    
    for input_file, dataset_name in datasets:
        if os.path.exists(input_file):
            try:
                result = process_dataset(spark, input_file, dataset_name)
                all_results.append(result)
            except Exception as e:
                print(f"Error processing {dataset_name}: {e}")
        else:
            print(f"File {input_file} not found, skipping...")
    
    save_results(all_results)
    
    spark.stop()
