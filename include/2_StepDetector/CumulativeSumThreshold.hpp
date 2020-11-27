#ifndef CUMULATIVESUM_THRESHOLD
#define CUMULATIVESUM_THRESHOLD

#include <vector>

class CumulativeSumThreshold {
public:
    // Constructors & Destructor
    CumulativeSumThreshold(double threshold_min, double cumsum_min, bool mode_mask=false);
    ~CumulativeSumThreshold();
    // Performing the thresholding o nhe input signal
    std::vector<bool> perform(const std::vector<double> & signal) const;
private:
    double threshold_min;
    double cumsum_min;
    bool mode_mask;
    bool deactivated_state;
    bool activated_state;
};

#endif /* CUMULATIVESUM_THRESHOLD */
