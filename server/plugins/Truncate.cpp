#include "SC_PlugIn.hpp"
#include "SC_DemandUnit.h"
#include <cmath>

// to "expose" std::trunc in sclang
// while maintaining backward compatibility
// because sclang's trunc is really floor(a/b) * b.
using std::trunc;
static InterfaceTable* ft;

// what do re nova?

class Truncate : public SCUnit {
public:
    Truncate() {
        switch (inRate(0)) {
        case calc_DemandRate:
            set_calc_function<Truncate, &Truncate::next_d>();
        case calc_ScalarRate:
            set_calc_function<Truncate, &Truncate::next_i>();
        // case calc_BufRate: // or simply set whole buf to inbuf[0] ?
        //     set_calc_function<Truncate, &Truncate::next_i>();
        default: // case calc_FullRate:
            set_calc_function<Truncate, &Truncate::next_a>();
        }
    }

private:
    void next_a(int inNumSamples) {
        const float* x = in(0);
        float* y = out(0);

        for (int i = 0; i < inNumSamples; i++) {
            y[i] = std::trunc(x[i]);
        }
    }
    void next_i(int inNumSamples) { out0(0) = trunc(in0(0)); }


    void next_d(int inNumSamples) {
        if (inNumSamples) {
            const float x = DemandInputA(this, 0, inNumSamples);
            out0(0) = sc_isnan(x) ? NAN : trunc(x);
        } else {
            ResetInput(this, 0);
        }
    }
};


PluginLoad(Truncate) {
    ft = inTable;
    registerUnit<Truncate>(ft, "Truncate", false);
}
