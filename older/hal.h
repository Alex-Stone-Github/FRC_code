

namespace hal {
    class ForceApplyer {
    public:
        virtual void set(double power) = 0;
        virtual void set_flipped(bool is_flipped) = 0;
    };
    class LimitSwitch {
    public:
        virtual bool get() = 0;
        virtual void set_flipped(bool is_flipped) = 0;
    };
    class VariableMeasurer {
    public:
        virtual double get() = 0;
        virtual void set_ticks_per_unit(double ratio) = 0;
    };
}
