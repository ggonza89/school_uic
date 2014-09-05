//
// C++ program to compute average scores
//
// Giancarlo Gonzalez
// U. of Illinois, Chicago
// CS341, Fall 2014
// Homework 1
//

#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <numeric>

using namespace std;

int main() {

	// reads in master.txt into a vector
    ifstream file("master.txt");
    istream_iterator<string> start(file), end;

    vector<string> master_file(start, end);
    vector< vector<int> > student_scores;

	// goes through each file on a line in master.txt
    auto score_at = 1, last_file_size = 0;

	if (!master_file.empty()) {

		for (auto & line : master_file) {

			cout << "Processing '" << line << "'..." << endl;
			ifstream temp_file(line);
			istream_iterator<int> student_start(temp_file), student_end;

			if (temp_file.fail()) {
				// if file does not exist it will quit and let you know
				cout << "...file does not exist" << endl;
				return 0;

			}

			// reads scores into a single vector
			vector< int > scores(student_start, student_end);
			// resizes student scores to make sure that each student has enough room to add a new column
			student_scores.resize(scores.size());

			// adds the score to the student_scores vector
			// cout << "student\tgrade" << endl;
			for (auto i = 0; i < scores.size(); ++i) {

				if (i == last_file_size && score_at != 1)
					// if this file contains more students make the previous score a 0 !ASSUMPTION!
					student_scores[i].push_back(0);

				student_scores[i].push_back(scores[i]);

				//     cout << i+1 << "\t" << student_scores[i][score_at-1] << endl;

			}

			last_file_size = scores.size();
			++score_at;

		}

		//cout << "student\tgrades" << endl;

		//for(auto it = 0; it < student_scores.size(); ++it) {

		//    //cout << it+1 << "\t";

		//    // cout << student_scores[it].size();
		//    for(auto grades = 0; grades < student_scores[it].size(); ++grades)
		//        cout << student_scores[it][grades] << " ";

		//    cout << endl;

		//}

		cout << "Student\tAverage" << endl;
		if (score_at <= 2) {

			for (auto student = 0; student < student_scores.size(); ++student)
				cout << student + 1 << "\t" << student_scores[student][0] << endl;

		}
		else {

			for (auto student = 0; student < student_scores.size(); ++student) {

				auto min_iterator = min_element(student_scores[student].begin(), student_scores[student].end());

				// cout << "student " << student+1 << " minimum score is " << student_scores[student][min_iterator] << endl;

				student_scores[student].erase(min_iterator);

				auto avg = double(accumulate(student_scores[student].begin(), student_scores[student].end(), 0)) / double(student_scores[student].size());

				cout << student + 1 << "\t" << avg << endl;

			}

		}

	}
	else
		cout << "No files to process, 'master.txt' is empty." << endl;
    // cout << "student\tgrades" << endl;

    // for(auto it = 0; it < student_scores.size(); ++it) {

    //     cout << it+1 << "\t";

    //     // cout << student_scores[it].size();
    //     for(auto grades = 0; grades < student_scores[it].size(); ++grades)
    //         cout << student_scores[it][grades] << " ";

    //     cout << endl;

    // }

    // cout << "student\taverages" << endl;

    // for(auto it = 0; it < student_scores.size(); ++it) {

    //     student_scores[it].push_back(accumulate(student_scores[it].begin(), student_scores[it].end(), 0)/student_scores[it].size());


    //     cout << it+1 << "\t" << student_scores[it].back() << endl;

    // }

    return 0;

}
