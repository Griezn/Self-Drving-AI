#pragma once
#include <cmath>
#include <vector>
#include <numeric>

class linear_regression
{
public:
	/// @brief Calculate the best linear fit to a set of points.
	/// @tparam T The type of the vectors entered.
	/// @tparam M The type for the slope and intercept.
	/// @param x The vector with the x values.
	/// @param y The vector with the y values.
	/// @param slope A reference to the place to store the calculated slope.
	/// @param intercept A reference to the place to store the calculated intercept.
	template<typename T, typename M>
	void fit(std::vector<T> x, std::vector<T> y, M &slope, M &intercept);

private:
	/// @brief Calculates the mean of a vector.	
	/// @tparam T The type of the vector values.
	/// @param vector The vector.
	/// @return The mean of the vector in the same type as the vector data.
	template<typename T>
	static T vector_mean(std::vector<T> vector);
	/// @brief Multiplies 2 vectors.
	/// @tparam T The type of the vectors.
	/// @param vector1 The first vector to multiply (order doesn't matter).  
	/// @param vector2 The second vector to multiply (order doesn't matter).  
	/// @return The multiplied vector.
	template<typename T>
	static std::vector<T> vector_multiply(std::vector<T> vector1, std::vector<T> vector2);
};

template <typename T, typename M>
void linear_regression::fit(std::vector<T> x, std::vector<T> y, M& slope, M& intercept){
	M x_mean = vector_mean(x);
	M y_mean = vector_mean(y);
	M ss_xy = 0;
	M ss_xx = 0;
	int n = x.size();

	{
		std::vector<T> temp;
		temp = vector_multiply(x, y);
		ss_xy = std::accumulate(temp.begin(), temp.end(), 0);
		ss_xy = ss_xy - n * x_mean * y_mean;
	}

	{
		std::vector<T> temp;
		temp = vector_multiply(x, x);
		ss_xx = std::accumulate(temp.begin(), temp.end(), 0);
		ss_xx = ss_xx - n * x_mean * x_mean;
	}

	slope = ss_xy / ss_xx;
	intercept = y_mean - slope * x_mean;
}

template <typename T>
T linear_regression::vector_mean(std::vector<T> vector){
	T n = vector.size();
	T sum;

	for (auto element : vector) {
		sum += element;
	}
	
	return sum / n;
}

template <typename T>
std::vector<T> linear_regression::vector_multiply(std::vector<T> vector1, std::vector<T> vector2){
	std::vector<T> return_vector;
	const auto n = vector1.size();

	for (int i = 0; i < n; ++i) {
		return_vector[i] = vector1[i] * vector2[i];
	}

	return return_vector;
}
