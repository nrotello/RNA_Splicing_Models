#include <iostream>
#include <fstream> 
#include <vector>
#include <numeric>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <typeinfo>

using namespace std;

// Set total sequences trained, model length, n choose 2, and file output/input names.
// Could in theory set that in one place.

int nt2int(char nt){
    if (nt == 'a'){return 0;}
	else if (nt == 'c'){return 1;}
	else if (nt == 'g'){return 2;}
	else if (nt == 't'){return 3;}
	else{return -1;}
    }

// TRAINING
void cam_training(string foreground_file, string background_file){

    int true_uni_nt[25][4] {};
    int true_di_nt[25][25][4][4] {};
	int true_tri_nt[25][25][25][4][4][4] {};

    int null_uni_nt[25][4] {};
    int null_di_nt[25][25][4][4] {};
    int null_tri_nt[25][25][25][4][4][4] {};


    // Initializing the multi-dimensional arrays with pseudocounts of 1.
    for (int pos1 = 0; pos1 < 25; pos1++){
        for (int base1 = 0; base1 < 4; base1 ++){
            true_uni_nt[pos1][base1] = 1.0;
            null_uni_nt[pos1][base1] = 1.0;

            for (int pos2 = 0; pos2 < 25; pos2++){
                for (int base2 = 0; base2 < 4; base2 ++){
                    true_di_nt[pos1][pos2][base1][base2] = 4.0;
                    null_di_nt[pos1][pos2][base1][base2] = 4.0;

                for (int pos3 = 0; pos3 < 25; pos3++){
                    for (int base3 = 0; base3 < 4; base3 ++){
                        true_tri_nt[pos1][pos2][pos3][base1][base2][base3] = 1.0;
                        null_tri_nt[pos1][pos2][pos3][base1][base2][base3] = 1.0;

                        }
                    }
                }
            }
        }
    }

    // End of pseudocounts

	// This checks if the file exists. If not, prints error.
	ifstream train_true(foreground_file);
	ifstream train_null(background_file);
    
    // Initialize input file to string and a vector with all the txt file contents/sequences.
	string splice_sites;

    vector<string> true_training;
	vector<string> null_training;

	int total_true{0};
	int total_null{0};

	// Will iterate through every line of txt and add X nt sequence substring to vector.
	while (getline(train_true, splice_sites)) {
		if (splice_sites.length() > 25){
			true_training.push_back(splice_sites.substr(splice_sites.length() - 25, 25));
			total_true ++;
			if (total_true == 100000){
				break;
			}
		}
	}

	while (getline(train_null, splice_sites)) {
		if (splice_sites.length() > 25){
			null_training.push_back(splice_sites.substr(splice_sites.length() - 25, 25));
			total_null ++;
			if (total_null == 100000){
				break;
			}
		}
	}

    cout << null_training.size() << endl << true_training.size() << endl;

    // Pulls sequence once.
	for (int sequence = 0; sequence < 100000; sequence++){
		string current_seq_true {true_training[sequence]};
		string current_seq_null {null_training[sequence]};

		// Then iterates through di-tri-nt.
		for (int pos1=0; pos1<25; pos1++){
			int first_base {nt2int(current_seq_true[pos1])};
			int first_base_null {nt2int(current_seq_null[pos1])};
            
			true_uni_nt[pos1][first_base] ++;
			null_uni_nt[pos1][first_base_null] ++;

			for (int pos2=pos1+1; pos2<25; pos2++){
				int second_base {nt2int(current_seq_true[pos2])};
				int second_base_null {nt2int(current_seq_null[pos2])};

				true_di_nt[pos1][pos2][first_base][second_base] ++;
				null_di_nt[pos1][pos2][first_base_null][second_base_null] ++;

				// Needs to have values for all third position for conditional models during eval.
				for (int pos3=0; pos3<25; pos3++){
					if (pos3 != pos2 and pos3 != pos1){
                        int third_base {nt2int(current_seq_true[pos3])};
                        int third_base_null {nt2int(current_seq_null[pos3])};

                        true_tri_nt[pos1][pos2][pos3][first_base][second_base][third_base] ++;
                        null_tri_nt[pos1][pos2][pos3][first_base_null][second_base_null][third_base_null] ++;
					}
				}
	
            }
        }
    }

    train_true.close();
	train_null.close();

	// Write arrays to text files. Must be read in the same format.
	std::ofstream true_di_file("25nt_true_di_array.txt");
	std::ofstream null_di_file("25nt_null_di_array.txt");
	std::ofstream true_tri_file("25nt_true_tri_array.txt");
	std::ofstream null_tri_file("25nt_null_tri_array.txt");

	// Split loops to write all four files at the same time.
	for (int pos1 = 0; pos1 < 25; pos1++){
		for (int pos2 = 0; pos2 < 25; pos2++){
			for (int base1 = 0; base1 < 4; base1++){
				for (int base2 = 0; base2 < 4; base2++){
					true_di_file << true_di_nt[pos1][pos2][base1][base2] << '\n';
					null_di_file << null_di_nt[pos1][pos2][base1][base2] << '\n';

					for (int pos3 = 0; pos3 < 25; pos3++){
						for (int base3 = 0; base3 < 4; base3++){
							true_tri_file << true_tri_nt[pos1][pos2][pos3][base1][base2][base3] << '\n';
							null_tri_file << null_tri_nt[pos1][pos2][pos3][base1][base2][base3] << '\n';
				        }
			        }

		        }
	        }
        }
    }

	true_di_file.close();
	null_di_file.close();
	true_tri_file.close();
	null_tri_file.close();

}

// EVALUATION

void cam_eval(string sequences2eval){

	int loaded_true_di_nt[25][25][4][4] {};
	int loaded_true_tri_nt[25][25][25][4][4][4] {};
	int loaded_null_di_nt[25][25][4][4] {};
	int loaded_null_tri_nt[25][25][25][4][4][4] {};

    ifstream inFile_true_di("25nt_true_di_array.txt");
	ifstream inFile_null_di("25nt_null_di_array.txt");
	ifstream inFile_true_tri("25nt_true_tri_array.txt");
	ifstream inFile_null_tri("25nt_null_tri_array.txt");

	string single_seq;
    vector<string> vector_true_di, vector_null_di, vector_true_tri, vector_null_tri;
	
	// Will iterate through every line of txt and add X nt sequence substring to vector.
	while (getline(inFile_true_di, single_seq)) {
			vector_true_di.push_back(single_seq);}
	while (getline(inFile_null_di, single_seq)) {
			vector_null_di.push_back(single_seq);}
	while (getline(inFile_true_tri, single_seq)) {
			vector_true_tri.push_back(single_seq);}
	while (getline(inFile_null_tri, single_seq)) {
			vector_null_tri.push_back(single_seq);}

    int order1 {0};
    int order2 {0};

	// Reorganize the vectors to the correct format. int order is how the txt is formatted above.
    for (int pos1 = 0; pos1 < 25; pos1++){
		for (int pos2 = 0; pos2 < 25; pos2++){
			for (int base1 = 0; base1 < 4; base1++){
				for (int base2 = 0; base2 < 4; base2++){

					loaded_true_di_nt[pos1][pos2][base1][base2] = stod(vector_true_di[order1]);
					loaded_null_di_nt[pos1][pos2][base1][base2] = stod(vector_null_di[order1]);
                    order1 ++;

					for (int pos3 = 0; pos3 < 25; pos3++){
						for (int base3 = 0; base3 < 4; base3++){

							loaded_true_tri_nt[pos1][pos2][pos3][base1][base2][base3] = stod(vector_true_tri[order2]);
					        loaded_null_tri_nt[pos1][pos2][pos3][base1][base2][base3] = stod(vector_null_tri[order2]);
                            order2 ++;
				        }
                    }
		        }
	        }
        }
    }

    inFile_true_di.close();
    inFile_null_di.close();
    inFile_null_tri.close();
    inFile_true_tri.close();

    // Scoring the sequence.

    ifstream file_w_sequences(sequences2eval);

	// Initialize input file to string and a vector with all the txt file contents/sequences.
	string single;
    vector<string> parsed_sequences;
	int total{0};

	// Will iterate through every line of txt and add X nt sequence substring to vector.
	while (getline(file_w_sequences, single)) {
		if (single.length() > 25){
			parsed_sequences.push_back(single.substr(single.length() - 25, 25));
			total ++;
		}
    }

    cout << total << endl;

    ofstream scores("scores.txt");
    
	for (string sequence:parsed_sequences){

		int counter{-1};
		// This needs parentheses not curly...
		vector<double> fore (300, 0.0);
		vector<double> back (300, 0.0);

		for (int pos1=0; pos1 < 25; pos1++){
			int base11 {nt2int(sequence[pos1])};
			for (int pos2 = pos1 + 1; pos2 < 25; pos2++){
				int base22 {nt2int(sequence[pos2])};

				double temp1 = loaded_true_di_nt[pos1][pos2][base11][base22];
				double temp2 = loaded_null_di_nt[pos1][pos2][base11][base22];
				counter ++;
				// Obtain p(XY) from counts / total + di-nt pseudocounts
				fore[counter] = temp1/100016.0;
				back[counter] = temp2/100016.0;

				for (int pos3=0; pos3 < 25; pos3++){

                    // Multiply all Z != XY P(XYZ)/P(XY)

					if (pos3 != pos1 and pos3 != pos2){
                        
						int base33 {nt2int(sequence[pos3])};
						// Including pseudocounts
                        double thing1 {loaded_true_tri_nt[pos1][pos2][pos3][base11][base22][base33]/100064.0};
                        double thing2 {loaded_null_tri_nt[pos1][pos2][pos3][base11][base22][base33]/100064.0};

                        fore[counter] *= (thing1 / (temp1/100016.0));
						back[counter] *= (thing2 / (temp2/100016.0));

                    }
			    }
		    }
	    }

		double summation1 = 0.0;
		double summation2 = 0.0;

		summation1 += reduce(fore.begin(), fore.end());
		summation2 += reduce(back.begin(), back.end());

        scores << log2(summation1/summation2) << endl;
	}

	file_w_sequences.close();
}

int main(){
	
    auto start = chrono::high_resolution_clock::now();

    cam_training("train3_true.txt", "train3_null.txt");
    auto stop = chrono::high_resolution_clock::now();
    cam_eval("test3_true.txt");

	auto duration = chrono::duration_cast<chrono::seconds>(stop - start);

	cout << "Time taken by function: "<< duration.count() << " seconds" << endl;
	return 0;
}

