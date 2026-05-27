module dx3d:graphics.rendersystem;
import std;
import :core;
import :win32;
import :com;

namespace dx3d
{
	struct RenderSystemDesc
	{
		BaseDesc Base;
	};
	class RenderSystem final : public Base
	{
	public:
		virtual ~RenderSystem() override = default;
		RenderSystem(const RenderSystemDesc& desc)
			: Base(desc.Base)
		{
			constexpr auto createDeviceFlags = 
				[] consteval -> unsigned
				{
					auto createDeviceFlags = 0u;
					if constexpr (IsDebug)
						createDeviceFlags |= D3D11::D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG;
					return createDeviceFlags;
				}();

			constexpr auto featureLevels = std::array{
				D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0,
			};
			auto createdFeatureLevel = D3D11::D3D_FEATURE_LEVEL{};
			auto hr = Win32::HRESULT{ 
				D3D11::D3D11CreateDevice(
					nullptr,
					D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
					nullptr,
					createDeviceFlags,
					featureLevels.data(),
					static_cast<Win32::UINT>(featureLevels.size()),
					D3D11::SdkVersion,
					device.ReleaseAndGetAddressOf(),
					&createdFeatureLevel,
					deviceContext.ReleaseAndGetAddressOf()
				) };
			if (Win32::Failed(hr))
				throw ComError{hr, "Failed to create D3D11 device and context." };
		}
	private:
		Ptr<D3D11::ID3D11Device> device;
		Ptr<D3D11::ID3D11DeviceContext> deviceContext;
	};
}