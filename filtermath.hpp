#pragma once

#include <cstddef>
#include <array>
#include <algorithm>
#include <ranges>
#include <cmath>
#include "utils.hpp"
#include <numeric>

struct z;
struct s;

template<typename Var>
struct InverseDef
{
	using type = InverseDef<Var>;
};

template<typename Var>
struct InverseDef<InverseDef<Var>>
{
	using type = Var;
};

template<typename Var>
using Inverse = InverseDef<Var>::type;

static_assert(std::is_same_v<Inverse<Inverse<z>>, z>);
static_assert(!std::is_same_v<Inverse<z>, z>);
static_assert(!std::is_same_v<Inverse<s>, z>);
static_assert(!std::is_same_v<Inverse<Inverse<s>>, z>);

template<typename Var, std::size_t Degree>
struct Polynomial
{
	std::array<double, Degree + 1> coefficients;

	template<std::size_t DegreeOther> requires (DegreeOther < Degree)
	constexpr Polynomial(const Polynomial<Var, DegreeOther>& other)
	{
		std::ranges::copy(other.coefficients, coefficients);
	}

	constexpr Polynomial() = default;
	constexpr Polynomial(const std::array<double, Degree + 1>& values) : coefficients{values}{}
};

template<typename Var, std::size_t DegreeA, std::size_t DegreeB>
[[nodiscard]] constexpr auto operator+(const Polynomial<Var, DegreeA>& polyA,
		                 const Polynomial<Var, DegreeB>& polyB)
{
	if constexpr (DegreeA < DegreeB) return polyB + polyA;
	else
	{
		auto result = polyA;
		for (std::size_t i = 0; i < polyB.coefficients.size(); ++i)
		{
			result.coefficients[i] += polyB.coefficients[i];
		}

		return result;
	}
}
static_assert((Polynomial<s,1>{{1.}} + Polynomial<s, 1>{{2., 3.}}).coefficients[1] == 3.);

template<typename Var, std::size_t P>
constexpr Polynomial<Var,P>& operator+=(Polynomial<Var, P>& polyA,
		                 const Polynomial<Var, P>& polyB)
{
	polyA = polyA + polyB;
	return polyA;
}

template<typename Var, std::size_t P, std::size_t Q>
[[nodiscard]] constexpr auto operator*(const Polynomial<Var, P>& polyA, const Polynomial<Var, Q>& polyB)
{
	Polynomial<Var, P + Q> result{};
	for (std::size_t i = 0; i < polyA.coefficients.size(); ++i)
	{
		for (std::size_t j = 0; j < polyB.coefficients.size(); ++j)
		{
			result.coefficients[i + j] += polyA.coefficients[i] * polyB.coefficients[j];
		}
	}
	return result;
}

static_assert((Polynomial<s, 1>{{1., 1.}} * Polynomial<s, 1>{{1., 1.}}).coefficients[1] == 2.);

template<typename Var, std::size_t P>
[[nodiscard]] constexpr auto operator*(double scalar, const Polynomial<Var, P>& poly)
{
	Polynomial<Var, P> result;
	std::ranges::transform(poly.coefficients, result.coefficients.begin(),
			               [&](double x){return scalar * x;});
	return result;
}

template<typename Var, std::size_t P>
[[nodiscard]] constexpr auto operator*(const Polynomial<Var, P>& poly, double scalar)
{
	return scalar * poly;
}

template<typename Var, std::size_t P, std::size_t Q>
[[nodiscard]] constexpr auto operator-(const Polynomial<Var, P>& polyA, const Polynomial<Var, Q>& polyB)
{
	return polyA + (-1 * polyB);
}

static_assert((3 * Polynomial<s, 1>{{1., 1.}}).coefficients[1] == 3.);

template<typename T>
constexpr bool always_false = false;
template<std::size_t Exp, typename Var, std::size_t P>
[[nodiscard]] constexpr auto Pow(const Polynomial<Var, P>& poly)
{
	if constexpr (Exp == 0)
	{
		return 1.;
	}
	else if constexpr (Exp == 1)
	{
		return poly;
	}
	else
	{
		return poly * Pow<Exp - 1>(poly);
	}
}
static_assert(Pow<3>(Polynomial<s, 1>({1., 1.})).coefficients[2] == 3.);

template<std::size_t P, std::size_t Q, typename Var>
struct TransferFunction
{
	Polynomial<Var, P> numerator;
	Polynomial<Var, Q> denominator;

	constexpr auto InvertVariable() const
	{
		//TODO: write simple proof as documentation
		TransferFunction<P, Q, Inverse<Var>> result;

		std::ranges::copy(numerator.coefficients | std::views::reverse, result.numerator.coefficients.begin());
		std::ranges::copy(denominator.coefficients | std::views::reverse, result.denominator.coefficients.begin());
		return result;
	}
};

template<std::size_t P, std::size_t Q>
using DiscreteTransferFunction = TransferFunction<P, Q, Inverse<z>>;

template<std::size_t P, std::size_t Q>
using ContinuousTransferFunction = TransferFunction<P, Q, Inverse<s>>;

template<std::size_t n, std::size_t N, std::size_t P>
[[nodiscard]] constexpr Polynomial<z, N> BilinearTransformSummand(const Polynomial<s, P>& poly, double K)
{
	return poly.coefficients[n]
		   * Pow<n>(K * Polynomial<z, 1>{{-1, 1}})
		   * Pow<N-n>(Polynomial<z, 1>{{1, 1}});
}

template<std::size_t N, std::size_t... is>
[[nodiscard]] constexpr auto helper(const Polynomial<s, sizeof...(is) - 1> poly, double K, std::index_sequence<is...>)
{
	return (BilinearTransformSummand<is, N>(poly, K) + ...);
}

template<std::size_t P, std::size_t Q>
[[nodiscard]] constexpr auto BilinearTransform(const TransferFunction<P, Q, s>& transferFunction, double samplerate)
{
	constexpr std::size_t N = std::max(P, Q);
	double K = 2. * samplerate;

	TransferFunction<N, N, z> result;

	result.numerator = helper<N>(transferFunction.numerator, K, std::make_index_sequence<P+1>{});
	result.denominator = helper<N>(transferFunction.denominator, K, std::make_index_sequence<Q+1>{});

	return result;
};

static constexpr std::array butterworthCoeff{1., 3.8637, 7.4641, 9.1416, 7.4641, 3.8637, 1.};

[[nodiscard]] constexpr TransferFunction<0, 6, s> ButterworthLowPass(double cutoffFrequency)
{
	TransferFunction<0, 6, s> result;
	result.numerator.coefficients[0] = 1.;
	std::ranges::transform(butterworthCoeff | std::views::enumerate, result.denominator.coefficients.begin(),
	    [&](const auto& pair){const auto& [i, x]{pair}; return x/std::pow(2 * 3.14159265 * cutoffFrequency, i);});

	return result;
}

[[nodiscard]] constexpr auto ButterworthHighPass(double cutoffFrequency)
{
	TransferFunction<6, 6, s> result;
	result.numerator.coefficients[6] = std::pow((1./(2*3.14159265*cutoffFrequency)), 6);
	std::ranges::transform(butterworthCoeff | std::views::enumerate, result.denominator.coefficients.begin(),
	    [&](const auto& pair){const auto& [i, x]{pair}; return x/std::pow(2 * 3.14159265 * cutoffFrequency, i);});

	return result;
}

template<std::size_t P, std::size_t Q>
struct DiscreteFilter
{
    std::array<double, P + 1> feedforward_coeff{};
    std::array<double, Q> feedback_coeff{};

	utils::Last<double, P + 1> last_inputs{};
	utils::Last<double, Q> last_outputs{};

    double operator()(double x)
    {
        last_inputs.push(x);

        double output = std::inner_product(feedforward_coeff.begin(), feedforward_coeff.end(), last_inputs.begin(), 0.)
                        - std::inner_product(feedback_coeff.begin(), feedback_coeff.end(), last_outputs.begin(), 0.);

        last_outputs.push(output);

		return output;
    }

	DiscreteFilter() = default;

	DiscreteFilter(const DiscreteTransferFunction<P, Q>& transferFunction)
		: DiscreteFilter()
	{
		SetTransferFunction(transferFunction);
	}

	void SetTransferFunction(const DiscreteTransferFunction<P, Q>& transferFunction)
	{
		auto normalize{[normFactor = transferFunction.denominator.coefficients[0]](double x)
			{
				return x / normFactor;
			}
		};
		std::ranges::transform(transferFunction.numerator.coefficients,
			feedforward_coeff.begin(), normalize);
		std::ranges::transform(transferFunction.denominator.coefficients | std::views::drop(1),
			feedback_coeff.begin(), normalize);
	}
};
