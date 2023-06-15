#pragma once

#include <type_traits>
#include <functional>
#include <utility>

enum class PortDirection {input, output};

template<PortDirection portDirection, typename... Ts>
class Port
{
	using OppositePortType = Port<portDirection == PortDirection::input ? PortDirection::output : PortDirection::input, Ts...>;
	friend OppositePortType;

	OppositePortType* connectedTo = nullptr;
	std::function<void(Ts...)> onReceiveCallback;

	template<typename... Args>
	void onReceive(Args&&... args) const
	requires ((std::is_convertible_v<Args, Ts> && ...) && portDirection == PortDirection::input)
	{
		//if (onReceiveCallback)
		//{
			onReceiveCallback(std::forward<Args>(args)...);
		//}
	}

public:
	template<typename F>
	void setCallback(F&& f)
	requires (portDirection == PortDirection::input)
	{
		onReceiveCallback = std::forward<F>(f);
	}

	void disconnect()
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

	bool isConnected() const
	{
		return connectedTo;
	}

	template<typename... Args>
	void send(Args&&... args) const
	requires (std::is_convertible_v<Args, Ts> && ...)
	{
		if (!this->connectedTo) throw;
		this->connectedTo->onReceive(args...);
	}

	// Shouldn't be able to have an input port without a callback,
	// so only output ports are allowed to be default-constructible.
	// Thus, the only available constructor for input ports is
	// the next one
	Port() requires (portDirection == PortDirection::output) = default;

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
