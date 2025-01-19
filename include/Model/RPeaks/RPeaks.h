#ifndef RPEAKS_H
#define RPEAKS_H

#include <vector>
#include <string>
#include <complex>
#include <cmath>

/**
 * @class RPeaks
 * @brief A class for detecting R-peaks in ECG signals using various algorithms.
 * 
 * This class provides methods for detecting R-peaks in ECG signals using the
 * Pan-Tompkins algorithm or the Hilbert transform method. It also allows for
 * customizable parameters to adjust the detection process.
 * 
 * Parameters
 * ----------
 * - detection_method: The detection method to use ("PAN_TOMPKINS" or "HILBERT").
 * - pan_tompkins_window_length: Window size for the Pan-Tompkins method.
 * - pan_tompkins_threshold: Threshold for the Pan-Tompkins method.
 * - hilbert_proximity: Minimum distance between R-peaks for the Hilbert method.
 * - hilbert_threshold: Threshold for R-peak detection in the Hilbert method.
 * - custom_parameters: Flag to check if custom parameters are set.
 * 
 * Methods
 * -------
 * - setParams: Configures parameters for the selected detection method.
 * - detectRPeaks: Detects R-peaks in the input signal.
 * - computeHilbert: Computes the Hilbert transform of a signal.
 * - filterPeaks: Filters detected peaks based on proximity and prominence.
 * - panTompkins: Implements the Pan-Tompkins algorithm for R-peak detection.
 * - hilbertTransform: Implements the Hilbert transform for R-peak detection.
 */
class RPeaks {
public:
    /**
     * @brief Default constructor initializing default parameters.
     */
    RPeaks();
    
    /**
     * @brief Sets parameters for the detection method.
     * @param method The detection method ("PAN_TOMPKINS" or "HILBERT").
     * @param window_size Window size for the Pan-Tompkins method (default: 0).
     * @param threshold Threshold for the Pan-Tompkins method (default: 0.0).
     * @param proximity Proximity for the Hilbert method (optional).
     */
    void setParams(const std::string& method, int window_size = 0, double threshold = 0.0, int proximity = 0);

    /**
     * @brief Detects R-peaks in the input signal.
     * @param signal The input signal (e.g., ECG).
     * @param signal_frequency The sampling frequency of the signal.
     * @param r_peaks Vector to store detected R-peak indices.
     * @return True if detection succeeds, false otherwise.
     */
    bool detectRPeaks(const std::vector<double>& signal, double signal_frequency, std::vector<int>& r_peaks);

private:
    // Detection parameters
    std::string detection_method;          ///< Detection method to use.
    int pan_tompkins_window_length;        ///< Window size for Pan-Tompkins method.
    double pan_tompkins_threshold;         ///< Threshold for Pan-Tompkins method.
    bool custom_parameters;                ///< Flag to check if custom parameters are set.
    int hilbert_proximity;                 ///< Proximity for Hilbert method, set by user or default.
    double hilbert_threshold;              ///< Threshold for Hilbert method, set by user or default.

    // Hilbert transform helper functions
    /**
     * @brief Computes the Hilbert transform of the input signal.
     * @param signal The input signal.
     * @return The analytic signal as a vector of complex numbers.
     */
    std::vector<std::complex<double>> computeHilbert(const std::vector<double>& signal);

    /**
     * @brief Filters peaks to remove close duplicates and retain the most prominent ones.
     * @param peaks The detected peak indices.
     * @param signal The original signal for comparison.
     * @param proximity Minimum distance between peaks.
     * @return Filtered peak indices.
     */
    std::vector<int> filterPeaks(const std::vector<int>& peaks, const std::vector<double>& signal, int proximity);

    // Detection methods
    /**
     * @brief Detects R-peaks using the Pan-Tompkins algorithm.
     * @param signal The input signal.
     * @param r_peaks Vector to store detected R-peak indices.
     * @param signal_frequency The sampling frequency of the signal.
     * @return True if detection succeeds, false otherwise.
     */
    bool panTompkins(const std::vector<double>& signal, std::vector<int>& r_peaks, double signal_frequency);

    /**
     * @brief Detects R-peaks using the Hilbert transform method.
     * @param signal The input signal.
     * @param r_peaks Vector to store detected R-peak indices.
     * @param signal_frequency The sampling frequency of the signal.
     * @return True if detection succeeds, false otherwise.
     */
    bool hilbertTransform(const std::vector<double>& signal, std::vector<int>& r_peaks, double signal_frequency);
};

#endif // RPEAKS_H
