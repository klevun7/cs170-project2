#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <algorithm>

using namespace std;

long long getCurrentTime() 
{
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

double defaultRate(const vector<int> &labels)
{
    int maxLabel = *max_element(labels.begin(), labels.end());
    int minLabel = *min_element(labels.begin(), labels.end());

    vector<int> counts(maxLabel - minLabel + 1, 0);
    for (int i = 0; i < labels.size(); i++)
    {
        counts[labels[i] - minLabel]++;
    }

    int majorityCount = 0;
    for (int i = 0; i < counts.size(); i++)
    {
        if (counts[i] > majorityCount)
        {
            majorityCount = counts[i];
        }
    }

    double accuracy = round(static_cast<double>(majorityCount) / labels.size() * 100.0 * 100) / 100;

    return accuracy;
}

vector<vector<double>> normalizeFeatures(const vector<vector<double>> &data)
{
    cout << "Please wait while I normalize the data... Done!" << endl;

    if (data.empty() || data[0].empty())
    {
        return data;
    }

    vector<vector<double>> normalized = data;
    int numFeatures = data[0].size();

    for (int j = 0; j < numFeatures; j++)
    {

        double minVal = data[0][j];
        double maxVal = data[0][j];

        for (int i = 1; i < data.size(); i++)
        {
            minVal = min(minVal, data[i][j]);
            maxVal = max(maxVal, data[i][j]);
        }

        for (int i = 0; i < data.size(); i++)
        {
            if (maxVal != minVal)
            {
                normalized[i][j] = (data[i][j] - minVal) / (maxVal - minVal);
            }
            else
            {
                normalized[i][j] = 0;
            }
        }
    }

    return normalized;
}

double evaluate(const vector<vector<double>> &data, const vector<int> &labels, const vector<int> &features)
{
    int correctPredictions = 0;

    long long calcStartTime = getCurrentTime();

    for (size_t i = 0; i < data.size(); ++i)
    {
        long long instanceStartTime = getCurrentTime();
        double minDistance = numeric_limits<double>::max();
        int predictedLabel = -1;

        for (size_t j = 0; j < data.size(); ++j)
        {
            if (i == j)
                continue;

            double distance = 0.0;
            for (int feature : features)
            {
                double diff = data[i][feature - 1] - data[j][feature - 1];
                distance += diff * diff;
            }

            distance = sqrt(distance);

            if (distance < minDistance)
            {
                minDistance = distance;
                predictedLabel = labels[j];
            }
        }
        /*
        cout << "Instance: " << i + 1
             << " | Predicted: " << predictedLabel
             << " | Actual: " << labels[i]
             << " | Time: " << (getCurrentTime() - instanceStartTime) << 
             " ms(micro)" << endl;
        */
        if (predictedLabel == labels[i])
        {
            correctPredictions = correctPredictions + 1;
        }
    }

    double accuracy = round(static_cast<double>(correctPredictions) / data.size() * 100.0 * 100) / 100;
    /*
    cout << "Results: " << correctPredictions << " / " << data.size() << endl;
    cout << "Accuracy: " << accuracy << "%" << endl;
    */
    return accuracy;
}

void forwardSelection(int numFeatures, const vector<vector<double>> &data, const vector<int> &labels)
{
    cout << "Beginning search.\n" << endl;

    vector<int> current_features;
    vector<int> best_overall_features;
    double best_overall_accuracy = 0.0;

    for (int i = 1; i <= numFeatures; i++)
    {
        int best_feature = -1;
        double best_accuracy = 0.0;

        for (int feature = 1; feature <= numFeatures; feature++)
        {
            bool already_selected = false;

            for (int j = 0; j < current_features.size(); j++)
            {
                if (current_features[j] == feature)
                {
                    already_selected = true;
                }
            }

            if (!already_selected)
            {
                vector<int> test_features = current_features;
                test_features.push_back(feature);

                double accuracy = evaluate(data, labels, test_features);
                cout << "Using feature(s): {";

                for (int j = 0; j < test_features.size(); j++)
                {
                    if (j != 0)
                    {
                        cout << ",";
                    }
                    cout << test_features[j];
                }

                cout << "} accuracy is " << accuracy << "%" << endl;

                if (accuracy > best_accuracy)
                {
                    best_accuracy = accuracy;
                    best_feature = feature;
                }
            }
        }

        if (best_feature != -1)
        {
            current_features.push_back(best_feature);
            cout << "\nFeature set {";

            for (int j = 0; j < current_features.size(); j++)
            {
                if (j != 0) 
                {
                    cout << ",";
                }
                cout << current_features[j];
            }

            cout << "} was best, accuracy is " << best_accuracy << "%\n"
                 << endl;

            if (best_accuracy > best_overall_accuracy)
            {
                best_overall_accuracy = best_accuracy;
                best_overall_features = current_features;
            }
            else
            {
                cout << "(Warning, Accuracy has decreased!)\n" << endl;
            }
        }
    }

    cout << "Finished search!! The best feature subset is {";

    for (int j = 0; j < best_overall_features.size(); j++)
    {
        if (j != 0) 
        {
            cout << ",";
        }
        cout << best_overall_features[j];
    }

    cout << "}, which has an accuracy of " << best_overall_accuracy << "%\n"
         << endl;
}

void backwardElimination(int numFeatures, const vector<vector<double>> &data, const vector<int> &labels)
{
    cout << "Beginning search.\n" << endl;

    vector<int> current_features;
    for (int i = 1; i <= numFeatures; i++)
    {
        current_features.push_back(i);
    }

    vector<int> best_overall_features;
    double best_overall_accuracy = evaluate(data, labels, current_features);

    cout << "Using features {";
    for (int j = 0; j < current_features.size(); j++)
    {
        if (j != 0)
        {
            cout << ",";
        }
        cout << current_features[j];
    }
    cout << "}, accuracy is " << best_overall_accuracy << "%\n"
         << endl;

    best_overall_features = current_features;

    while (current_features.size() > 1)
    {
        int feature_to_remove = -1;
        double best_accuracy = 0.0;

        for (int j = 0; j < current_features.size(); j++)
        {
            vector<int> test_features = current_features;
            test_features.erase(test_features.begin() + j);

            double accuracy = evaluate(data, labels, test_features);
            cout << "Using feature(s): {";

            for (int k = 0; k < test_features.size(); k++)
            {
                if (k != 0)
                {
                    cout << ",";
                }
                cout << test_features[k];
            }

            cout << "} accuracy is " << accuracy << "%" << endl;

            if (accuracy > best_accuracy || feature_to_remove == -1)
            {
                best_accuracy = accuracy;
                feature_to_remove = j;
            }
        }

        int removed_feature = current_features[feature_to_remove];
        current_features.erase(current_features.begin() + feature_to_remove);

        cout << "\nFeature set {";
        for (int j = 0; j < current_features.size(); j++)
        {
            if (j != 0)
            {
                cout << ",";
            }
            cout << current_features[j];
        }
        cout << "} was best, accuracy is " << best_accuracy << "%\n"
             << endl;

        if (best_accuracy > best_overall_accuracy)
        {
            best_overall_accuracy = best_accuracy;
            best_overall_features = current_features;
        }
        else
        {
            cout << "(Warning, Accuracy has decreased!)\n" << endl;
        }
    }

    cout << "Finished search!! The best feature subset is {";
    for (int j = 0; j < best_overall_features.size(); j++)
    {
        if (j != 0)
        {
            cout << ",";
        }
        cout << best_overall_features[j];
    }
    cout << "}, which has an accuracy of " << best_overall_accuracy << "%\n"
         << endl;
}

int main()
{
    cout << "Welcome to Kevin and Duy Feature Selection Algorithm" << endl;

    vector<vector<double>> data;
    vector<int> labels;
    string fileName;

    cout << "Type in the name of the file to test : ";

    getline(cin, fileName);
    ifstream inputFile(fileName);
    if (!inputFile)
    {
        cout << "Error opening file. Exiting program." << endl;
        return 1;
    }

    string line;
    while (getline(inputFile, line))
    {
        istringstream iss(line);
        double value;
        vector<double> row;

        if (!(iss >> value))
        {
            continue;
        }
        labels.push_back(static_cast<int>(value));

        while (iss >> value)
        {
            row.push_back(value);
        }

        if (!row.empty())
        {
            data.push_back(row);
        }
    }

    if (data.empty())
    {
        cout << "No data was read from file." << endl;
        return 1;
    }

    cout << "This dataset has " << data[0].size() << " features (not including the class attribute), with " << data.size() << " instances." << endl;

    data = normalizeFeatures(data);
/*
    vector<int> testFeatures;
    if (data[0].size() == 10)
    {
        testFeatures = {3, 5, 7};
    }
    else if (data[0].size() == 40)
    { 
        testFeatures = {1, 15, 27};
    }
    else if (data[0].size() == 6)
    { 
        testFeatures = {2};
    }


    if (!testFeatures.empty())
    {
        double accuracy = evaluate(data, labels, testFeatures);
        cout << "Testing specified feature set {";
        for (int f : testFeatures)
        {
            cout << " " << f;
        }
        cout << " }, accuracy: " << accuracy << "%" << endl;
    }
*/
    int numFeatures = data[0].size();
    int choice = 0;

    while (true)
    {
        cout << "Type the number of the algorithm you want to run." << endl;
        cout << "1. Forward Selection" << endl;
        cout << "2. Backward Elimination" << endl;

        cin >> choice;

        if (cin.fail())
        {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "Invalid input. Please enter a number (1 or 2)." << endl;
            continue; 
        }

        if (choice == 1)
        {
            cout << "Running nearest neighbor with no features (default rate), using “leaving-one-out” evaluation, I get an accuracy of " << defaultRate(labels) << "%\n" << endl;
            forwardSelection(numFeatures, data, labels);
            break;
        }
        else if (choice == 2)
        {
            cout << "Running nearest neighbor with no features (default rate), using “leaving-one-out” evaluation, I get an accuracy of " << defaultRate(labels) << "%\n" << endl;
            backwardElimination(numFeatures, data, labels);
            break;
        }
        else
        {
            cout << "Invalid choice. Please select either 1 or 2." << endl;
        }
    }

    return 0;
}