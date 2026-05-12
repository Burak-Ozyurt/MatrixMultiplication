#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <stdexcept>

using namespace std;
using namespace std::chrono;

class MatrixException : public runtime_error {
public:
    MatrixException(const string& msg) : runtime_error(msg) {}
};

inline int multiplyElements(int a, int b) {
    return a * b;
}

vector<vector<int>> readMatrix(ifstream& file, int expectedRows, int expectedCols) {
    vector<vector<int>> matrix(expectedRows, vector<int>(expectedCols));
    string line;
    int i = 0;
    
    while (i < expectedRows) {
        if (!getline(file, line)) throw MatrixException("Satir eksik: Beklenen boyuttan daha az satir var.");
        if (line.empty()) continue; // Boş satırları atla
        
        stringstream ss(line);
        string temp;
        int j = 0;
        
        while (ss >> temp) {
            if (j >= expectedCols) throw MatrixException("Satirda fazladan eleman: Beklenenden fazla veri girilmis.");
            
            for (char c : temp) {
                if (!isdigit(c) && c != '-') throw MatrixException("Sayisal olmayan deger saptandi: " + temp);
            }
            matrix[i][j] = stoi(temp);
            j++;
        }
        if (j < expectedCols) throw MatrixException("Satirda eksik eleman: Beklenenden daha az veri girilmis.");
        i++;
    }
    return matrix;
}

void parseAndValidateFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) { cerr << "Dosya bulunamadi: " << filename << endl; return; }

    try {
        int n, m, p, q;
        if (!(file >> n >> m)) throw MatrixException("Matris A boyutlari okunamadi.");
        string dummy; getline(file, dummy);
        
        vector<vector<int>> A = readMatrix(file, n, m);

        while (file.peek() == '\n' || file.peek() == '\r' || file.peek() == ' ') file.ignore();
        
        if (!(file >> p >> q)) throw MatrixException("Matris B boyutlari okunamadi.");
        getline(file, dummy);
        
        if (m != p) throw MatrixException("Uyumsuz matris boyutlari: Matris A'nin sutun sayisi (" + to_string(m) + "), Matris B'nin satir sayisina (" + to_string(p) + ") esit degil.");

        vector<vector<int>> B = readMatrix(file, p, q);
        cout << filename << " basariyla okundu ve dogrulandi. (" << n << "x" << m << ") x (" << p << "x" << q << ")" << endl;
    } catch (const MatrixException& e) {
        cerr << "Hata (" << filename << "): " << e.what() << endl;
    }
}

void multiplyVectorDirect(const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C, int n, int m, int q) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < q; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < m; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void multiplyVectorFunc(const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C, int n, int m, int q) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < q; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < m; ++k) {
                C[i][j] += multiplyElements(A[i][k], B[k][j]);
            }
        }
    }
}

void multiplyDynamicArray(int** A, int** B, int** C, int n, int m, int q) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < q; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < m; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void runBenchmarks() {
    vector<pair<int, int>> sizes = {{50, 80}, {120, 150}, {300, 400}, {900, 700}};
    int num_runs = 5;

    cout << left << setw(15) << "Language" << setw(25) << "Implementation" << setw(15) << "Size" << "Avg. Time (ms)" << endl;
    cout << string(75, '-') << endl;

    for (auto size : sizes) {
        int n = size.first, m = size.second, p = m, q = n + 10;
        string sizeStr = to_string(n) + "x" + to_string(m);

        vector<vector<int>> vA(n, vector<int>(m, 2)), vB(p, vector<int>(q, 3)), vC(n, vector<int>(q, 0));

        auto start = high_resolution_clock::now();
        for(int r=0; r<num_runs; r++) multiplyVectorDirect(vA, vB, vC, n, m, q);
        auto stop = high_resolution_clock::now();
        cout << left << setw(15) << "C++" << setw(25) << "Vector (Direct)" << setw(15) << sizeStr << duration_cast<milliseconds>(stop - start).count()/num_runs << endl;

        start = high_resolution_clock::now();
        for(int r=0; r<num_runs; r++) multiplyVectorFunc(vA, vB, vC, n, m, q);
        stop = high_resolution_clock::now();
        cout << left << setw(15) << "C++" << setw(25) << "Vector (Func Call)" << setw(15) << sizeStr << duration_cast<milliseconds>(stop - start).count()/num_runs << endl;

        int** dA = new int*[n]; for(int i=0; i<n; i++) { dA[i] = new int[m]; fill_n(dA[i], m, 2); }
        int** dB = new int*[p]; for(int i=0; i<p; i++) { dB[i] = new int[q]; fill_n(dB[i], q, 3); }
        int** dC = new int*[n]; for(int i=0; i<n; i++) dC[i] = new int[q];

        start = high_resolution_clock::now();
        for(int r=0; r<num_runs; r++) multiplyDynamicArray(dA, dB, dC, n, m, q);
        stop = high_resolution_clock::now();
        cout << left << setw(15) << "C++" << setw(25) << "Dynamic Array" << setw(15) << sizeStr << duration_cast<milliseconds>(stop - start).count()/num_runs << endl;

        for(int i=0; i<n; i++) { delete[] dA[i]; delete[] dC[i]; }
        for(int i=0; i<p; i++) delete[] dB[i];
        delete[] dA; delete[] dB; delete[] dC;
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        parseAndValidateFile(argv[1]);
    } else {
        cout << "Benchmarking başlıyor...\n" << endl;
        runBenchmarks();
    }
    return 0;
}