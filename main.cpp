#include <iostream>
#include <vector>
#include <cmath>
#include <limits>

using namespace std;

vector<vector<double>> normalizeFeatures(const vector<vector<double>> &data) {}

double evaluate(const vector<vector<double>> &data, const vector<int> &labels, const vector<int> &features) {
    int correctPredictions = 0;

    for (size_t i = 0; i < data.size(); ++i) {
        double minDistance = numeric_limits<double>::max();
        int predictedLabel = -1;

        for (size_t j = 0; j < data.size(); ++j) {
            if (i == j) continue;

            double distance;
            for (int i = 0; i < features.size(); i++) {
                int feature = features[i]; 
                distance = distance + pow(data[i][feature - 1] - data[j][feature - 1], 2);
            }
            distance = sqrt(distance);
            if (distance < minDistance) {
                minDistance = distance;
                predictedLabel = labels[j];
            }
        }

        if (predictedLabel == labels[i]) {
            correctPredictions = correctPredictions + 1;
        }
    }

    return static_cast<double>(correctPredictions) / data.size() * 100.0;
}


void forwardSelection(int numFeatures, const vector<vector<double>> &data, const vector<int> &labels) 
{
    cout << "Beginning search." << endl;

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
                cout << "Using feature(s): { ";

                for (int j = 0; j < test_features.size(); j++)
                {
                    cout << test_features[j] << " ";
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
            cout << "Feature set { ";

            for (int j = 0; j < current_features.size(); j++)
            {
                cout << current_features[j] << " ";
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
                cout << "(Warning, Accuracy has decreased!)" << endl;
            }
        }
    }

    cout << "Finished search!! The best feature subset is { ";

    for (int j = 0; j < best_overall_features.size(); j++)
    {
        cout << best_overall_features[j] << " ";
    }

    cout << "}, which has an accuracy of " << best_overall_accuracy << "%\n"
         << endl;
}

void backwardElimination(int numFeatures, const vector<vector<double>> &data, const vector<int> &labels) 
{
    cout << "Beginning search." << endl;

    vector<int> current_features;
    for (int i = 1; i <= numFeatures; i++)
    {
        current_features.push_back(i);
    }

    vector<int> best_overall_features;
    double best_overall_accuracy = evaluate(data, labels, current_features);

    cout << "Using all features { ";
    for (int j = 0; j < current_features.size(); j++)
    {
        cout << current_features[j] << " ";
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
            cout << "Using feature(s): { ";

            for (int k = 0; k < test_features.size(); k++)
            {
                cout << test_features[k] << " ";
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

        cout << "Feature set { ";
        for (int j = 0; j < current_features.size(); j++)
        {
            cout << current_features[j] << " ";
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
            cout << "(Warning, Accuracy has decreased!)" << endl;
        }
    }

    cout << "Finished search!! The best feature subset is { ";
    for (int j = 0; j < best_overall_features.size(); j++)
    {
        cout << best_overall_features[j] << " ";
    }
    cout << "}, which has an accuracy of " << best_overall_accuracy << "%\n"
         << endl;
}

int main()
{
    cout << "Welcome to Kevin and Duy Forward Selection Algorithm" << endl;

    vector<vector<double>> data;
    vector<int> labels;

    // read file here

    data = normalizeFeatures(data);
    int numFeatures = data[0].size();

    cout << "Type the number of the algorithm you want to run." << endl;
    cout << "1. Forward Selection" << endl;
    cout << "2. Backward Elimination" << endl;

    int choice;
    cin >> choice;

    if (choice == 1)
    {
        cout << "Using no features and random evaluation, I get an accuracy of " << (rand() % 10000) / 100.0 << "%" << endl;
        forwardSelection(numFeatures, data, labels);
    }
    else if (choice == 2)
    {
        cout << "Using no features and random evaluation, I get an accuracy of " << (rand() % 10000) / 100.0 << "%" << endl;
        backwardElimination(numFeatures, data, labels);
    }
    else
    {
        cout << "Invalid choice. Please select either 1 or 2." << endl;
    }

    return 0;
}