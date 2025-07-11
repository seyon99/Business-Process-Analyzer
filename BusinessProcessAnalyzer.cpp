#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <chrono>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <iomanip>

using namespace std;
using namespace std::chrono;

struct Process {
    string id;
    string ownerId;
    string type;
    string priority; // HIGH, MEDIUM, LOW
    system_clock::time_point startTime;
    system_clock::time_point endTime;
    string status;
    vector<string> stepsCompleted;
    int retries;
    bool isAutomated;
    map<string, string> metadata;
};

class BusinessProcessAnalyzer {
private:
    vector<Process> processes;
    vector<double> weights = {0, 0, 0, 0, 0}; // w0, w1, w2, w3, w4

    double toSeconds(system_clock::time_point start, system_clock::time_point end) const {
        return duration_cast<seconds>(end - start).count();
    }

    int priorityValue(const string& p) const {
        if (p == "HIGH") return 3;
        if (p == "MEDIUM") return 2;
        return 1;
    }

public:
    void addProcess(const Process& p) {
        processes.push_back(p);
    }

    // Forecast duration using trained weights
    double forecastDuration(const Process& p) const {
        double x1 = p.retries;
        double x2 = p.stepsCompleted.size();
        double x3 = priorityValue(p.priority);
        double x4 = p.isAutomated ? 1 : 0;

        return weights[0] + weights[1]*x1 + weights[2]*x2 + weights[3]*x3 + weights[4]*x4;
    }

    // Predict ETA of a running process
    system_clock::time_point predictETA(const Process& p) const {
        double forecastedDuration = forecastDuration(p);
        auto start = p.startTime;
        return start + seconds(static_cast<int>(forecastedDuration));
    }

    // Train linear regression model on completed data
    void trainForecastModel() {
        // Variables for X and y
        vector<vector<double>> X;
        vector<double> y;

        for (const auto& p : processes) {
            if (p.status != "COMPLETED") continue;

            double dur = toSeconds(p.startTime, p.endTime);
            y.push_back(dur);

            X.push_back({
                1.0,                              // bias term
                static_cast<double>(p.retries),
                static_cast<double>(p.stepsCompleted.size()),
                static_cast<double>(priorityValue(p.priority)),
                static_cast<double>(p.isAutomated ? 1 : 0)
            });
        }

        if (X.empty()) return;

        // Normal equations: w = (XᵗX)^-1 Xᵗy
        int m = X.size(), n = X[0].size();
        vector<vector<double>> XtX(n, vector<double>(n, 0.0));
        vector<double> Xty(n, 0.0);

        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                Xty[j] += X[i][j] * y[i];
                for (int k = 0; k < n; ++k) {
                    XtX[j][k] += X[i][j] * X[i][k];
                }
            }
        }

        // Solve XtX * w = Xty using Gaussian elimination
        vector<vector<double>> A(n, vector<double>(n + 1, 0.0));
        for (int i = 0; i < n; ++i) {
            copy(XtX[i].begin(), XtX[i].end(), A[i].begin());
            A[i][n] = Xty[i];
        }

        // Gaussian elimination
        for (int i = 0; i < n; ++i) {
            // Pivot
            int maxRow = i;
            for (int k = i+1; k < n; ++k)
                if (abs(A[k][i]) > abs(A[maxRow][i]))
                    maxRow = k;
            swap(A[i], A[maxRow]);

            // Eliminate
            for (int k = i+1; k < n; ++k) {
                double factor = A[k][i] / A[i][i];
                for (int j = i; j <= n; ++j)
                    A[k][j] -= factor * A[i][j];
            }
        }

        // Back substitution
        weights = vector<double>(n, 0.0);
        for (int i = n-1; i >= 0; --i) {
            weights[i] = A[i][n] / A[i][i];
            for (int k = 0; k < i; ++k)
                A[k][n] -= A[k][i] * weights[i];
        }
    }

    void printWeights() const {
        cout << "Trained weights:\n";
        for (size_t i = 0; i < weights.size(); ++i) {
            cout << "w" << i << ": " << weights[i] << "\n";
        }
    }
};
