#pragma once

#include <circle/logger.h>
#include "utils.hpp"
#include <numeric>
#include "module.hpp"

template<std::size_t sizeA, std::size_t sizeB = 0>
struct DiscreteFilterModule : Module<PortID<"signalIn", PortDirection::input, double>,
						  PortID<"signalOut", PortDirection::output, double>
						 >
{
    std::array<double, sizeA + 1> feedforward_coeff{};
    std::array<double, sizeB> feedback_coeff{};

	utils::Last<double, sizeA + 1> last_inputs;
	utils::Last<double, sizeB> last_outputs;

    void signalInCallback(double x)
    {
        last_inputs.push(x);

        double output = std::inner_product(feedforward_coeff.begin(), feedforward_coeff.end(), last_inputs.begin(), 0.)
                        - std::inner_product(feedback_coeff.begin(), feedback_coeff.end(), last_outputs.begin(), 0.);

        last_outputs.push(output);

        getPort<"signalOut">().send(output);
    }

	DiscreteFilterModule()
	{
		getPort<"signalIn">().setCallback([this](double signal){signalInCallback(signal);});
	}
};
