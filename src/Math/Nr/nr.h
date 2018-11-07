#ifndef _MATH_NR_NR_H_
#define _MATH_NR_NR_H_

#include <fstream>
#include <complex>
#include <Math/Matrix.hh>
#include <Math/Vector.hh>
#include "nrutil.h"
#include "nrtypes.h"

namespace Math { namespace Nr {

    typedef Math::Matrix<DP,Math::VectorRangeCheckOffPolicy> CoreMatrix;

    DP midpnt(const Nr::FunctorBase<DP,DP>& func, const DP a, const DP b, const int n);
    DP qromb(const Nr::FunctorBase<DP,DP>& func, DP a, DP b);
    DP qromo(const Nr::FunctorBase<DP,DP>& func, const DP a, const DP b,
	     DP choose(const Nr::FunctorBase<DP,DP>&, const DP, const DP, const int));
    DP qsimp(const FunctorBase<DP,DP>& func, const DP a, const DP b);
    DP qtrap(const Nr::FunctorBase<DP,DP>& func, const DP a, const DP b);
    DP pythag(const DP a, const DP b);
    void rkck(Vec_I_DP &y, Vec_I_DP &dydx, const DP x,
	      const DP h, Vec_O_DP &yout, Vec_O_DP &yerr,
	      Nr::DerivativesBase<DP,Vec_I_DP,Vec_O_DP> &derivs);
    void rkqs(Vec_IO_DP &y, Vec_IO_DP &dydx, DP &x, const DP htry,
	      const DP eps, Vec_I_DP &yscal, DP &hdid, DP &hnext,
	      Nr::DerivativesBase<DP,Vec_I_DP,Vec_O_DP> &derivs);
    void ludcmp(CoreMatrix &a, Math::Vector<int>  &indx, DP &d);
    void lubksb(CoreMatrix &a, Math::Vector<int>  &indx, Math::Vector<DP> &b);
    void svdcmp(CoreMatrix &a, Math::Vector<DP> &w, CoreMatrix  &v);
    void svbksb(const CoreMatrix &u, const Math::Vector<DP>  &w, const CoreMatrix &v,
		const Math::Vector<DP> &b, Math::Vector<DP> &x);
    DP trapzd(const FunctorBase<DP,DP>& func, const DP a, const DP b, const int n);


    /** modified bessel function of first kind and zeroth order (i0) */
    f64 bessi0(f64 x);


    /** four1: Fast Fourier transformation
     * @param data input and output buffer, Re and Im alternately
     * @param isign 1 transformation, -1 inverse transformation
     */
    void four1(vector<f32> &data, const s32 isign);

    /** realft: Fourier transformation for real vectors.
     * @param data input and output buffer;
     *   remark: real part of last data moved/should be moved to imaginary part of first data
     * @param isign 1 transformation, -1 inverse transformation
     */
    void realft(vector<f32> &data, const s32 isign);

    /** evlmem: maximum entropy (all-poles) power spectrum estimation.
     *  P(omega) = gain^2 / |1 + a1 e^(omega Ts) + a2 e^(omega Ts * 2) - ... - aN e^(omega Ts * N)|^2
     *
     * @param gain is gain of all-poles (also called autoregressive) model,
     * @param "a" is all-pole coefficients,
     * @param normalizedFrequency (normally [0..0.5]) is f / fs, where f is frequency and
     *   fs is sample rate in time domain.
     */
    float evlmem(float normalizedFrequency, const std::vector<float> &a, float gain);

#if 0
    /** ODEInt */
    class ODEInt{
      public:

	typedef Math::Matrix<DP,Math::VectorRangeCheckOffPolicy> CoreMatrix;

      private:

	DP dxsav;
	int kmax,kount;
	Vec_DP *xp_p;
	Mat_DP *yp_p;
	bool toBeDeleted;
	int nok, nbad;
	Vec_DP ystart;

	Nr::Vec_DP odeint(const Vec_IO_DP &ystart, const DP x1, const DP x2, const DP eps,
			  const DP h1, const DP hmin, Nr::DerivativesBase<DP,Vec_I_DP,Vec_O_DP> & derivs,
			  void rkqs(Vec_IO_DP &, Vec_IO_DP &, DP &, const DP, const DP,
				    Vec_I_DP &, DP &, DP &,
				    Nr::DerivativesBase<DP,Vec_I_DP,Vec_O_DP> &)=rkqs);

      public:

	ODEInt();
	ODEInt(int _kmax, int N);
	~ODEInt();

	const Vec_DP& xIntermediate() const {return *xp_p;}
	const Mat_DP& yIntermediate() const {return *yp_p;}
	void intermediateIntervall(DP _dxsav)  {dxsav=_dxsav;}
	int numberOfIntemediateResults()const{return kount;}
	void setStartValues(const Vec_DP& _ystart){ystart=_ystart;}

	inline Nr::Vec_DP integrate(Nr::DerivativesBase<DP,Vec_I_DP,Vec_O_DP> & derivs,
				    DP x1, DP x2,
				    DP stepsize, DP precision= 1.0e-5,
				    DP stepsizeMin=0.0){
	    return odeint(ystart,x1, x2, precision , stepsize, stepsizeMin, derivs);
	}
    };
#endif
} } // namespace Math::Nr
#endif /* _MATH_NR_NR_H_ */
