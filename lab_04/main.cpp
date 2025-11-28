#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <queue>
#include <bitset>
#include <stdexcept>
#include <zlib.h>
#include <chrono>
#include <iomanip>
#include <dirent.h>
#include <sys/stat.h>


class HuffmanNode {
public:
    unsigned char ch;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(const unsigned char character, const int frequency)
        : ch(character), freq(frequency), left(nullptr), right(nullptr) {}

    ~HuffmanNode() {
        delete left;
        delete right;
    }
};

class HuffmanCompression {
public:
    static void compress(const std::string& inputFile, const std::string& outputFile);
    static void decompress(const std::string& inputFile, const std::string& outputFile);

private:
    struct Compare {
        bool operator()(const HuffmanNode* l, const HuffmanNode* r) const {
            return l->freq > r->freq;
        }
    };

    static HuffmanNode* buildHuffmanTree(const std::map<unsigned char, int>& freqMap);
    static void buildCodes(const HuffmanNode* root, const std::string& str, std::map<unsigned char, std::string>& huffmanCode);
    static void writeCompressedFile(std::ofstream& outFile, const std::map<unsigned char, std::string>& huffmanCode, const std::string& inputFile);
    static void writeFrequencyTable(std::ofstream& outFile, const std::map<unsigned char, int>& freqMap);
    static std::map<unsigned char, int> readFrequencyTable(std::ifstream& inFile);
    static std::string readEncodedData(std::ifstream& inFile);
};

HuffmanNode* HuffmanCompression::buildHuffmanTree(const std::map<unsigned char, int>& freqMap) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, Compare> pq;

    for (const auto& p : freqMap)
        pq.push(new HuffmanNode(p.first, p.second));

    while (pq.size() > 1) {
        HuffmanNode* left = pq.top();
        pq.pop();
        HuffmanNode* right = pq.top();
        pq.pop();

        auto* node = new HuffmanNode(0, left->freq + right->freq);
        node->left = left;
        node->right = right;
        pq.push(node);
    }

    return pq.empty() ? nullptr : pq.top();
}

void HuffmanCompression::buildCodes(const HuffmanNode* root, const std::string& str, std::map<unsigned char, std::string>& huffmanCode) {
    if (!root)
        return;

    if (!root->left && !root->right) {
        huffmanCode[root->ch] = str.empty() ? "0" : str;
        return;
    }

    buildCodes(root->left, str + "0", huffmanCode);
    buildCodes(root->right, str + "1", huffmanCode);
}

void HuffmanCompression::writeFrequencyTable(std::ofstream& outFile, const std::map<unsigned char, int>& freqMap) {
    uint32_t size = freqMap.size();
    outFile.write(reinterpret_cast<char*>(&size), sizeof(size));

    for (const auto& p : freqMap) {
        outFile.put(p.first);
        outFile.write(reinterpret_cast<const char*>(&p.second), sizeof(p.second));
    }
}

std::map<unsigned char, int> HuffmanCompression::readFrequencyTable(std::ifstream& inFile) {
    std::map<unsigned char, int> freqMap;
    uint32_t size;
    int freq;

    if (!inFile.read(reinterpret_cast<char*>(&size), sizeof(size)))
        return {};

    for (uint32_t i = 0; i < size; ++i) {
        unsigned char ch = inFile.get();
        inFile.read(reinterpret_cast<char*>(&freq), sizeof(freq));
        freqMap[ch] = freq;
    }

    return freqMap;
}

void HuffmanCompression::writeCompressedFile(std::ofstream& outFile, const std::map<unsigned char, std::string>& huffmanCode, const std::string& inputFile) {
    std::ifstream inFile(inputFile, std::ios::binary);
    if (!inFile)
        throw std::runtime_error("Cannot open input file for compression");

    std::string encodedData;
    char ch;
    while (inFile.get(ch))
        encodedData += huffmanCode.at(ch);
    inFile.close();

    const size_t originalBitLength = encodedData.size();
    while (encodedData.size() % 8)
        encodedData += '0';

    outFile.write(reinterpret_cast<const char*>(&originalBitLength), sizeof(originalBitLength));

    for (size_t i = 0; i < encodedData.size(); i += 8) {
        std::bitset<8> byte(encodedData.substr(i, 8));
        outFile.put(static_cast<unsigned char>(byte.to_ulong()));
    }
}

std::string HuffmanCompression::readEncodedData(std::ifstream& inFile) {
    size_t originalBitLength;
    inFile.read(reinterpret_cast<char*>(&originalBitLength), sizeof(originalBitLength));

    std::string encodedData;
    unsigned char byte;

    while (inFile.read(reinterpret_cast<char*>(&byte), 1)) {
        std::bitset<8> bits(byte);
        encodedData += bits.to_string();
    }

    encodedData = encodedData.substr(0, originalBitLength);

    return encodedData;
}

void HuffmanCompression::compress(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream inFile(inputFile, std::ios::binary);
    if (!inFile)
        throw std::runtime_error("Cannot open input file");

    std::map<unsigned char, int> freqMap;
    unsigned char byte;
    while (inFile.read(reinterpret_cast<char*>(&byte), 1))
        freqMap[byte]++;
    inFile.close();

    std::ofstream outFile(outputFile, std::ios::binary);

    if (freqMap.empty()) {
        outFile.close();
        return;
    }

    HuffmanNode* root = buildHuffmanTree(freqMap);
    std::map<unsigned char, std::string> huffmanCode;
    buildCodes(root, "", huffmanCode);

    writeFrequencyTable(outFile, freqMap);
    writeCompressedFile(outFile, huffmanCode, inputFile);
    outFile.close();

    delete root;
}

void HuffmanCompression::decompress(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream inFile(inputFile, std::ios::binary);
    if (!inFile)
        throw std::runtime_error("Cannot open input file");

    std::ofstream outFile(outputFile, std::ios::binary);

    std::map<unsigned char, int> freqMap = readFrequencyTable(inFile);
    if (freqMap.empty()) {
        inFile.close();
        outFile.close();
        return;
    }

    std::string encodedData = readEncodedData(inFile);
    inFile.close();

    HuffmanNode* root = buildHuffmanTree(freqMap);
    if (!root)
        throw std::runtime_error("Failed to rebuild Huffman tree");

    if (!root->left && !root->right)
        for (size_t i = 0; i < encodedData.size(); ++i)
            outFile.put(root->ch);
    else {
        HuffmanNode* current = root;

        for (char bit : encodedData) {
            current = (bit == '0') ? current->left : current->right;

            if (!current->left && !current->right) {
                outFile.put(current->ch);
                current = root;
            }
        }
    }

    outFile.close();
    delete root;
}

double calculateCompressionCoeff(const std::string& originalFile, const std::string& compressedFile) {
    std::ifstream original(originalFile, std::ios::binary | std::ios::ate);
    if (!original)
        throw std::runtime_error("Cannot open original file");
    std::streamsize originalSize = original.tellg();
    original.close();

    std::ifstream compressed(compressedFile, std::ios::binary | std::ios::ate);
    if (!compressed)
        throw std::runtime_error("Cannot open compressed file");
    std::streamsize compressedSize = compressed.tellg();
    compressed.close();

    if (originalSize == 0)
        return 0.0;

    return static_cast<double>(originalSize) / static_cast<double>(compressedSize);
}

double zlibCompress(const std::string& inputFile, const std::string& outputFile) {
    auto start = std::chrono::high_resolution_clock::now();

    std::ifstream inFile(inputFile, std::ios::binary);
    if (!inFile)
        throw std::runtime_error("Cannot open input file for zlib compression");

    std::ofstream outFile(outputFile, std::ios::binary);
    if (!outFile)
        throw std::runtime_error("Cannot open output file for zlib compression");

    std::vector<char> input((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    std::vector<char> output(compressBound(input.size()));

    uLongf compressedSize = output.size();
    if (compress(reinterpret_cast<Bytef*>(output.data()), &compressedSize, reinterpret_cast<const Bytef*>(input.data()), input.size()) != Z_OK)
        throw std::runtime_error("zlib compression failed");

    outFile.write(output.data(), compressedSize);

    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

double zlibDecompress(const std::string& inputFile, const std::string& outputFile) {
    auto start = std::chrono::high_resolution_clock::now();

    std::ifstream inFile(inputFile, std::ios::binary);
    if (!inFile)
        throw std::runtime_error("Cannot open input file for zlib decompression");

    std::ofstream outFile(outputFile, std::ios::binary);
    if (!outFile)
        throw std::runtime_error("Cannot open output file for zlib decompression");

    std::vector<char> input((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    std::vector<char> output(input.size() * 4);

    uLongf decompressedSize = output.size();
    int result = uncompress(reinterpret_cast<Bytef*>(output.data()), &decompressedSize, reinterpret_cast<const Bytef*>(input.data()), input.size());

    while (result == Z_BUF_ERROR) {
        output.resize(output.size() * 2);
        decompressedSize = output.size();
        result = uncompress(reinterpret_cast<Bytef*>(output.data()), &decompressedSize, reinterpret_cast<const Bytef*>(input.data()), input.size());
    }

    if (result != Z_OK)
        throw std::runtime_error("zlib decompression failed");

    outFile.write(output.data(), decompressedSize);

    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

void saveResultsToCSV(const std::string& filePath, const std::string& algorithm, const std::string& file, double compressionRatio, double encodeTime, double decodeTime) {
    std::ofstream csvFile(filePath, std::ios::app);
    if (!csvFile)
        throw std::runtime_error("Cannot open CSV file");

    csvFile << algorithm << "," << file << "," << std::fixed << std::setprecision(6)
            << compressionRatio << "," << encodeTime << "," << decodeTime << "\n";
}

static std::string baseName(const std::string& path) {
    size_t pos = path.find_last_of("/");
    if (pos == std::string::npos) return path;
    return path.substr(pos + 1);
}

static void processFile(const std::string& inputFile, const std::string& csvFile, const std::string& outDir) {
    std::string bn = baseName(inputFile);

    std::string huffEnc = outDir + "/huff.enc." + bn;
    std::string huffDec = outDir + "/huff.dec." + bn;
    std::string zlibEnc = outDir + "/zlib.enc." + bn;
    std::string zlibDec = outDir + "/zlib.dec." + bn;

    // Huffman
    double huffmanEncodeTime = 0.0;
    double huffmanDecodeTime = 0.0;
    double huffmanCompressionRatio = 0.0;

    auto start = std::chrono::high_resolution_clock::now();
    HuffmanCompression::compress(inputFile, huffEnc);
    auto end = std::chrono::high_resolution_clock::now();
    huffmanEncodeTime = std::chrono::duration<double, std::milli>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    HuffmanCompression::decompress(huffEnc, huffDec);
    end = std::chrono::high_resolution_clock::now();
    huffmanDecodeTime = std::chrono::duration<double, std::milli>(end - start).count();

    huffmanCompressionRatio = calculateCompressionCoeff(inputFile, huffEnc);
    saveResultsToCSV(csvFile, "Huffman", inputFile, huffmanCompressionRatio, huffmanEncodeTime, huffmanDecodeTime);

    // zlib
    double zlibEncodeTime = zlibCompress(inputFile, zlibEnc);
    double zlibDecodeTime = zlibDecompress(zlibEnc, zlibDec);
    double zlibCompressionRatio = calculateCompressionCoeff(inputFile, zlibEnc);
    saveResultsToCSV(csvFile, "zlib", inputFile, zlibCompressionRatio, zlibEncodeTime, zlibDecodeTime);
}

int main(int argc, char** argv) {
    try {
        const std::string outDir = "out";
        struct stat st_out;
        if (stat(outDir.c_str(), &st_out) != 0) {
            if (mkdir(outDir.c_str(), 0755) != 0) {
                throw std::runtime_error("Cannot create output directory: " + outDir);
            }
        } else if (!S_ISDIR(st_out.st_mode)) {
            throw std::runtime_error(outDir + " exists and is not a directory");
        }

        const std::string csvFile = outDir + "/results.csv";

        std::ofstream header(csvFile, std::ios::trunc);
        if (!header)
            throw std::runtime_error("Cannot create CSV file");
        header << "algorithm,file,compression_ratio,encode_time_ms,decode_time_ms\n";
        header.close();

        const std::string dataDir = "data";
        DIR* dir = opendir(dataDir.c_str());
        if (!dir) {
            throw std::runtime_error("Cannot open data directory: " + dataDir);
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string name = entry->d_name;
            if (name.size() >= 5 && name.rfind("file.", 0) == 0) {
                std::string fullpath = dataDir + "/" + name;

                struct stat st;
                if (stat(fullpath.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
                    std::cout << "[Processing]: " << fullpath << std::endl;
                    processFile(fullpath, csvFile, outDir);
                }
            }
        }
        closedir(dir);

        std::cout << "[OK]: saved to " << csvFile << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}