int main() {
    BusinessProcessAnalyzer analyzer;
    auto now = system_clock::now();

    analyzer.addProcess({
        "p1", "user1", "TRADE", "HIGH", now - seconds(600), now - seconds(300),
        "COMPLETED", {"INIT", "VERIFY", "EXECUTE"}, 2, true, {}
    });

    analyzer.addProcess({
        "p2", "user2", "TRANSFER", "MEDIUM", now - seconds(1000), now - seconds(600),
        "COMPLETED", {"INIT", "ROUTE"}, 1, false, {}
    });

    analyzer.addProcess({
        "p3", "user3", "AUDIT", "LOW", now - seconds(1200), now - seconds(900),
        "COMPLETED", {"INIT", "LOG"}, 0, false, {}
    });

    analyzer.trainForecastModel();
    analyzer.printWeights();

    Process newProc = {
        "pX", "user4", "TRADE", "HIGH", now, now,
        "IN_PROGRESS", {"INIT", "VERIFY"}, 1, true, {}
    };

    double forecastedSeconds = analyzer.forecastDuration(newProc);
    auto eta = analyzer.predictETA(newProc);
    time_t etaTime = system_clock::to_time_t(eta);

    cout << "Forecasted duration: " << forecastedSeconds << " sec\n";
    cout << "Estimated completion time: " << ctime(&etaTime);
}
