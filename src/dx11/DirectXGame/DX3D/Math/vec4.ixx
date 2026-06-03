export module dx3d:math.vec4;

export namespace dx3d
{
	struct Vec4
	{
		float x{};
		float y{};
		float z{};
		float w{};
		constexpr auto Data() const noexcept -> const float*
		{
			return &x;
		}
	};
}