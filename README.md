# Business Process Analyzer
A C++ program that takes in data about business processes (e.g., process ID, start time, end time, and status), and forecasts outcomes

## Core Functionality
1. Predict the duration of a new/incomplete process based on:
  - type
  = priority
  - stepsCompleted.size()
  - retries
2. Estimate ETA if it’s currently running.
3. Support linear regression-style learning based on historical data.
