#ifndef HYSTERESIS_THRESHOLD
#define HYSTERESIS_THRESHOLD

#include <vector>

class HysteresisThreshold {
public:
    // Constructor
    HysteresisThreshold(double DeactivationThreshold, double ActivationThreshold, bool mode_mask=false, bool quick_activation=false, bool quick_deactivation=false);
    // Destructor
    ~HysteresisThreshold();
    std::vector<bool> perform(const std::vector<double> & signal) const;
private:
    double activation;
    double deactivation;
    int getState(double value) const;
    bool mode_mask;
    bool quick_activation;
    bool quick_deactivation;
    bool deactivated_state;
    bool activated_state;
};

#endif /* HYSTERESIS_THRESHOLD */
