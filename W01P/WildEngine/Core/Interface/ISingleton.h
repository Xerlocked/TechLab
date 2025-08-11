#pragma once

template <typename T>
class ISingleton
{
public:
	static T& GetInst()
	{
		static T Instance;
		return Instance;
	}

protected:
	ISingleton() = default;
	virtual ~ISingleton() = default;

	// 복사 및 이동 방지
	ISingleton(const ISingleton&) = delete;
	ISingleton& operator=(const ISingleton&) = delete;
	ISingleton(ISingleton&&) = delete;
	ISingleton& operator=(ISingleton&&) = delete;
};

