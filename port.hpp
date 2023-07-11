#pragma once

#include <type_traits>
#include <functional>
#include <utility>
#include "utils.hpp"

enum class PortDirection {input, output};

class BasePort
{
public:
	[[nodiscard]] virtual bool canConnectTo(const BasePort&) const = 0;
	[[nodiscard]] virtual bool isConnected() const = 0;

	virtual void connectTo(BasePort&) = 0;
	virtual void disconnect() = 0;
};

template<PortDirection portDirection, typename... Ts>
class Port : public BasePort
{
	using OppositePortType = Port<portDirection == PortDirection::input ? PortDirection::output : PortDirection::input, Ts...>;
	friend OppositePortType;

	OppositePortType* connectedTo = nullptr;
	std::function<void(Ts...)> onReceiveCallback;

	template<typename... Args>
	void onReceive(Args&&... args) const
	requires ((std::is_convertible_v<Args, Ts> && ...) && portDirection == PortDirection::input)
	{
		if (onReceiveCallback)
		{
			onReceiveCallback(std::forward<Args>(args)...);
		}
	}

public:
	template<typename F>
	void setCallback(F&& f)
	requires (portDirection == PortDirection::input)
	{
		onReceiveCallback = std::forward<F>(f);
	}

	[[nodiscard]] bool canConnectTo(const BasePort& other) const override
	{
		return dynamic_cast<const OppositePortType*>(&other);
	}

	void disconnect() override
	{
		if (!connectedTo) return;

		connectedTo->connectedTo = nullptr;
		connectedTo = nullptr;
	}

	void connectTo(OppositePortType& other)
	{
		if (connectedTo)
		{
			if (&other != connectedTo) throw;
			else return;
		}
		connectedTo = &other;
		connectedTo->connectedTo = this;
	}

	void connectTo(BasePort& other) override
	{
		connectTo(dynamic_cast<OppositePortType&>(other));
	}

	[[nodiscard]] bool isConnected() const override
	{
		return connectedTo;
	}

	template<typename... Args>
	void send(Args&&... args) const
	requires (portDirection == PortDirection::output)
	{
		if (!this->connectedTo) throw;
		this->connectedTo->onReceive(args...);
	}

	Port() = default;

	template<typename F>
	requires (portDirection == PortDirection::input)
	Port(F&& f) : onReceiveCallback{std::forward<F>(f)} {};

	Port(const Port&) = delete;
	Port& operator=(const Port&) = delete;

	Port(Port&& other) noexcept
	{
		if (!other.connectedTo) return;

		connectedTo = std::exchange(other.connectedTo, nullptr);

		if (connectedTo) connectedTo->connectedTo = this;
	}

	Port& operator=(Port&& other) noexcept
	{
		if (this != &other)
		{
			disconnect();

			connectedTo = std::exchange(other.connectedTo, nullptr);

			if (connectedTo) connectedTo->connectedTo = this;
		}
		return *this;
	}

	~Port()
	{
		disconnect();
	}
};

template<utils::ConstexprString _name, PortDirection _portDirection, typename... Ts>
struct PortID
{
	static constexpr std::string_view name{_name};
	static constexpr PortDirection portDirection{_portDirection};
	using PortType = Port<portDirection, Ts...>;
};

template<typename T>
concept PortIDType = requires (T x)
{
	{PortID{x}} -> std::same_as<T>;
};
