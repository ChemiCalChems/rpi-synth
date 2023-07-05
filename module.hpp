#pragma once

#include <concepts>
#include <tuple>
#include <type_traits>

#include "port.hpp"
#include "utils.hpp"

namespace detail
{

template<utils::ConstexprString name, std::size_t N, PortIDType id, PortIDType... ids>
[[nodiscard]] constexpr auto& getPortHelper(auto& ports)
{
	if constexpr(id::name == name)
	{
		return std::get<N>(ports);
	}
	else
	{
		static_assert(sizeof...(ids) > 0);
		return getPortHelper<name, N+1, ids...>(ports);
	}
}

template<std::size_t N, PortIDType id, PortIDType... ids>
[[nodiscard]] constexpr auto getPortHelper(auto& ports, std::string_view name)
	-> std::conditional_t<std::is_const_v<std::remove_reference_t<decltype(ports)>>, const BasePort, BasePort>&
{
	if (id::name == name) return std::get<N>(ports);
	else if constexpr (sizeof...(ids) > 0) return getPortHelper<N+1, ids...>(ports, name);
	else throw;
}

} // namespace detail

template<PortIDType... ids>
struct Module
{
protected:
	std::tuple<typename ids::PortType...> ports;
public:
	template<utils::ConstexprString name>
	[[nodiscard]] constexpr auto& getPort()
	{
		return detail::getPortHelper<name, 0, ids...>(ports);
	}

	template<utils::ConstexprString name>
	[[nodiscard]] constexpr const auto& getPort() const
	{
		return detail::getPortHelper<name, 0, ids...>(ports);
	}

	[[nodiscard]] constexpr BasePort& getPort(std::string_view name)
	{
		return detail::getPortHelper<0, ids...>(ports, name);
	}

	[[nodiscard]] constexpr const BasePort& getPort(std::string_view name) const
	{
		return detail::getPortHelper<0, ids...>(ports, name);
	}
};
