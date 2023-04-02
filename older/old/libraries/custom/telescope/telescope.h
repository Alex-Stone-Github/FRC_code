#ifndef TELESCOPE_H
#define TELESCOPE_H

#include <iostream>
#include <cmath>

struct ArmControlPacketInfo {
    double joint_angle;
    double extension_percent;
};
class ArmSimulation {
private:
    double m_mast_length;
    double m_joint_angle; // radians
    double m_extension_percent;
    double m_min_extension_length;
    double m_max_extension_length;
public:
    ArmSimulation(double mast_len, double min_ext, double max_ext): m_mast_length(mast_len),
    m_min_extension_length(min_ext), m_max_extension_length(max_ext) {
        m_extension_percent = 0;
        m_joint_angle = 0;
    }
    ArmControlPacketInfo move_to_point(double x, double y) {
        double dx = 0 - x;
        double dy = y - m_mast_length;
        double angle = std::atan2(dy, dx);
        double max_ext_dist = m_max_extension_length - m_min_extension_length;
        double dist_out = std::sqrt(std::pow(dx, 2) + std::pow(dy, 2));
        double needed_extension_dist = dist_out - m_min_extension_length;
        if (needed_extension_dist < 0) {needed_extension_dist = 0;} // clamp extension dist to be at least zero
        double needed_extension_percent = needed_extension_dist / max_ext_dist;
        if (needed_extension_percent > 1) {needed_extension_percent = 1;}
        return {
            angle,
            needed_extension_percent
        };
    }
};

#endif // TELESCOPE_H
