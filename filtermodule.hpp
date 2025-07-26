#pragma once

#include <circle/logger.h>
#include "utils.hpp"
#include <numeric>
#include "module.hpp"
#include "filtermath.hpp"

template<std::size_t sizeA, std::size_t sizeB = 0>
struct DiscreteFilterModule : Module<PortID<"signalIn", PortDirection::input, double>,
						  PortID<"signalOut", PortDirection::output, double>
						 >
{
	DiscreteFilter<sizeA, sizeB> filter;

    void signalInCallback(double x)
    {
        getPort<"signalOut">().send(filter(x));
    }

	DiscreteFilterModule()
	{
		getPort<"signalIn">().setCallback([this](double signal){signalInCallback(signal);});
	}

	DiscreteFilterModule(const DiscreteTransferFunction<sizeA, sizeB>& transferFunction)
		: DiscreteFilterModule()
	{
		SetTransferFunction(transferFunction);
	}

	void SetTransferFunction(const DiscreteTransferFunction<sizeA, sizeB>& transferFunction)
	{
		filter.SetTransferFunction(transferFunction);
	}
};
