#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "Kernel32.lib")

#include <directx/d3d12.h>
#include <directx/d3dx12.h>
#include <d3dcompiler.h>
import std;

template<typename T>
struct ComPtr
{
	constexpr ~ComPtr()
	{
		Close();
	}

	struct OwnPtr { T* Ptr = nullptr; };
	struct CopyPtr { T* Ptr = nullptr; };
	
	constexpr ComPtr() noexcept = default;
	constexpr ComPtr(OwnPtr own) noexcept : Ptr(own.Ptr) {}
	
	constexpr ComPtr(CopyPtr copy) noexcept
		: Ptr(copy.Ptr)
	{
		if (Ptr)
			Ptr->AddRef();
	}

	constexpr ComPtr(const ComPtr& other) noexcept { Copy(other); };
	constexpr auto& operator=(this auto&& self, const ComPtr& other) noexcept
	{
		self.Copy(other);
		return *this;
	}

	constexpr ComPtr(ComPtr&& other) noexcept { Move(other); }
	constexpr auto& operator=(this auto&& self, ComPtr&& other) noexcept
	{
		self.Move(other);
		return *this;
	}

	constexpr auto operator->(this auto&& self) noexcept -> T* { return self.Ptr; }
	constexpr auto operator&(this auto&& self) noexcept -> T** { return &self.Ptr; }
	constexpr auto VoidAddress(this auto&& self) noexcept -> void** { return reinterpret_cast<void**>(&self.Ptr); }

	constexpr void Copy(this auto&& self, const ComPtr& other)
	{
		self.Close();
		if (other.Ptr)
		{
			self.Ptr = other.Ptr;
			self.Ptr->AddRef();
		}
	}

	constexpr void Move(this auto&& self, ComPtr&& other)
	{
		self.Close();
		if (other.Ptr)
		{
			self.Ptr = other.Ptr;
			other.Ptr = nullptr;
		}
	}

	constexpr auto Close(this auto&& self)
	{
		if (self.Ptr)
		{
			self.Ptr->Release();
			self.Ptr = nullptr;
		}
	}

	T* Ptr = nullptr;
};


struct DxError : std::runtime_error
{
	DxError(HRESULT hr, const std::source_location& loc = std::source_location::current()) 
		: std::runtime_error(std::format("{} -> {}:{}:{}", hr, loc.function_name(), loc.file_name(), loc.line()))
	{ }
};

int main(int argc, char* argv[])
try
{
	// Step 1: acquire device
	ComPtr<ID3D12Device> d3d12Device;
	HRESULT hr = D3D12CreateDevice(
		nullptr, // use default adapter
		D3D_FEATURE_LEVEL_12_1, // https://docs.microsoft.com/en-us/windows/desktop/api/d3dcommon/ne-d3dcommon-d3d_feature_level
		__uuidof(ID3D12Device),
		d3d12Device.VoidAddress()
	);
	if (FAILED(hr))
		throw DxError(hr);

	// Step 2: arrange feature levels and presentation
	constexpr std::array FeatureLevels
	{ 
		D3D_FEATURE_LEVEL_12_0,	// Then check for 12 support
		D3D_FEATURE_LEVEL_11_0, // Then check for API 11 support
		D3D_FEATURE_LEVEL_10_0,	// And so on...
		D3D_FEATURE_LEVEL_9_3
	};

	using Pair = std::pair<D3D_FEATURE_LEVEL, std::string_view>;
	constexpr std::array FeatureLevel
	{
		Pair{ D3D_FEATURE_LEVEL_12_1, "D3D12.1"},
		Pair{ D3D_FEATURE_LEVEL_12_0, "D3D12.0"},
		Pair{ D3D_FEATURE_LEVEL_11_0, "D3D11.0"},
		Pair{ D3D_FEATURE_LEVEL_10_0, "D3D10.0"},
		Pair{ D3D_FEATURE_LEVEL_9_3, "D3D9.3"}
	};

	// Step 3: populate request struct
	D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevelsInfo{
		.NumFeatureLevels = static_cast<UINT>(FeatureLevels.size()),
		.pFeatureLevelsRequested = FeatureLevels.data()
	};

	// Step 4: request information 
	hr = d3d12Device->CheckFeatureSupport(
		D3D12_FEATURE_FEATURE_LEVELS, 
		&featureLevelsInfo, 
		sizeof(featureLevelsInfo)
	);
	if (FAILED(hr))
		throw DxError(hr);

	std::println("Checking support for the following feature levels:");
	for (const auto& [level, name] : FeatureLevel)
		std::println("    {}", name);

	auto result = std::ranges::find_if(
		FeatureLevel, 
		[max = featureLevelsInfo.MaxSupportedFeatureLevel](const auto& pair)
		{
			return pair.first == max;
		});
	if (result == FeatureLevel.end())
		throw std::runtime_error("Not found");

	// Step 5: print information
	std::println("\nHighest feature level supported: {}", result->second);
	return 0;
}
catch (const std::exception& ex)
{
	std::println("{}", ex.what());
	return 1;
}
