#include <iostream>
#include <cstdio>
#include <string>
#include <vector>

const int N = 1000;
const std::string macdOut = "pliki tekstowe/MACD.txt";
const std::string signalOut = "pliki tekstowe/SIGNAL.txt";

class Macd {

public:
    std::string inputFileName = "";
	FILE* input = NULL;
	std::vector<double> samples = {};

	FILE* macdData = fopen(macdOut.c_str(), "w");
	FILE* signalData = fopen(signalOut.c_str(), "w");

public:
    Macd(const std::string inputFileName) {
        this->inputFileName = inputFileName;
    }

	int openFile() {
		this->input = fopen(this->inputFileName.c_str(), "r");
		if (input == nullptr) {
			printf("[Blad]: Brak pliku wejsciowego");
			return -1;
		}
		return 0;
	}

	int getInputData() {

		int linesCount = 0;
		while (feof(this->input) == 0) {

			float data;
			if (fscanf(this->input, "%f", &data) != 1) {
				printf("Error: input file contains unexpected symbols\n");
				fclose(input);
				return -1;
			}
			this->samples.push_back((double)data);
			linesCount++;
		}
		if (linesCount != N) {
			printf("Error: wrong count of lines in input file. Need value is %d, current - %d\n", N, linesCount);
			fclose(input);
			return -1;
		}

		return 0;
	}

	double exponential_moving_average(int current_day, int day_count, std::vector<double> smpl) {
		double a = 0.0;
		double b = 0.0;
		double one_minus_alpha = 1.0 - (2.0/(day_count - 1));
		for (int i = 0; i <= day_count; ++i)
		{
			double tmp = pow(one_minus_alpha, i);
			if (current_day - i > 0)
				a += tmp * smpl.at(current_day - 1 - i);
			else
				a += tmp * smpl.at(0);

			b += tmp;
		}
		return a / b;
	}

	double moving_average_convergence_divergence(int periodic) {
		double macd = exponential_moving_average(periodic, 12, this->samples) - 
			exponential_moving_average(periodic, 26, this->samples);
		return macd;
	}

	double macd_signal(int periodic, std::vector<double> macdSamples)
	{
		double ema = exponential_moving_average(periodic, 9, macdSamples);
		return ema;
	}

	void countMacdSignal() {

		std::vector<double> macdSamples = {};
		for (int i = 1; i <= N; ++i) {

			double macd = moving_average_convergence_divergence(i);
			macdSamples.push_back(macd);
			double signal = macd_signal(i, macdSamples);

			fprintf(this->macdData, "%f\n", macd);
			fprintf(this->signalData, "%f\n", signal);
		}
	}
};

int main() {
    
	const std::string inputFileName = "pliki tekstowe/wig20_data_29.03.2018-31.03.2022.txt";
	Macd* data = new Macd(inputFileName);

	if (data->openFile() == -1 || data->getInputData() == -1) return -1;
	data->countMacdSignal();

    return 0;
}