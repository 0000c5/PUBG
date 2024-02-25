#include "types.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

UINT width = 0, height = 0;

class fname {
public:
	fname() : comparison_index(uint32_t()), number(int32_t()) {};
	fname(uint32_t index) : comparison_index(index), number(int32_t()) {};

	uint32_t comparison_index;
	int32_t number;
};

class fkey {
public:
	fkey() : name() {}
	fkey(fname name) : name(name) {}

public:
	fname name;
	uint8_t details[24] = {};
};

class ftextdata {
public:
	char pad_0[0x30];
	wchar_t* name;
	int length;
};

class ftext {
public:
	ftextdata* data;
	char pad_0[0x10];

	wchar_t* c_str() const {
		if (this->data) {
			return data->name;
		}

		return nullptr;
	}
};

class frotator {
public:
	frotator() : pitch(0.f), yaw(0.f), roll(0.f) {}
	frotator(float pitch, float yaw) : pitch(pitch), yaw(yaw), roll(0.f) {}
	frotator(float pitch, float yaw, float roll) : pitch(pitch), yaw(yaw), roll(roll) {}

	frotator operator + (const frotator& other) const { return { this->pitch + other.pitch, this->yaw + other.yaw, this->roll + other.roll }; }
	frotator operator - (const frotator& other) const { return { this->pitch - other.pitch, this->yaw - other.yaw, this->roll - other.roll }; }
	frotator operator * (float scalar) const { return { this->pitch * scalar, this->yaw * scalar, this->roll * scalar }; }
	frotator operator / (float divide) const { return { this->pitch / divide, this->yaw / divide, this->roll / divide }; }

	frotator& operator *= (const float other) { this->yaw *= other; this->pitch *= other; this->roll *= other; return *this; }
	frotator& operator /= (const float other) { this->yaw /= other; this->pitch /= other; this->roll /= other; return *this; }

	frotator& operator =  (const frotator& other) { this->pitch = other.pitch; this->yaw = other.yaw; this->roll = other.roll; return *this; }
	frotator& operator += (const frotator& other) { this->pitch += other.pitch; this->yaw += other.yaw; this->roll += other.roll; return *this; }
	frotator& operator -= (const frotator& other) { this->pitch -= other.pitch; this->yaw -= other.yaw; this->roll -= other.roll; return *this; }
	frotator& operator *= (const frotator& other) { this->pitch *= other.pitch; this->yaw *= other.yaw; this->roll *= other.roll; return *this; }
	frotator& operator /= (const frotator& other) { this->pitch /= other.pitch; this->yaw /= other.yaw; this->roll /= other.roll; return *this; }

	operator bool() {
		return bool(this->pitch && this->yaw && this->roll);
	}

	friend bool operator==(const frotator& first, const frotator& second) {
		return first.pitch == second.pitch && first.yaw == second.yaw && first.roll == second.roll;
	}

	friend bool operator!=(const frotator& first, const frotator& second) {
		return !(first == second);
	}

public:
	float pitch = 0.f;
	float yaw = 0.f;
	float roll = 0.f;
};

struct fvector2d {
	float x, y;

	fvector2d() : x(0), y(0) {}

	fvector2d(float x, float y) : x(x), y(y) {}

	fvector2d operator + (const fvector2d& other) const { return fvector2d(x + other.x, y + other.y); }

	fvector2d operator- (const fvector2d& other) const { return fvector2d(x - other.x, y - other.y); }

	fvector2d operator* (float scalar) const { return fvector2d(x * scalar, y * scalar); }

	fvector2d operator/ (float scalar) const { return fvector2d(x / scalar, y / scalar); }

	fvector2d& operator=  (const fvector2d& other) { x = other.x; y = other.y; return *this; }

	fvector2d& operator+= (const fvector2d& other) { x += other.x; y += other.y; return *this; }

	fvector2d& operator-= (const fvector2d& other) { x -= other.x; y -= other.y; return *this; }

	fvector2d& operator*= (const float other) { x *= other; y *= other; return *this; }

	friend bool operator==(const fvector2d& one, const fvector2d& two) { return one.x == two.x && one.y == two.y; }

	friend bool operator!=(const fvector2d& one, const fvector2d& two) { return !(one == two); }

	friend bool operator>(const fvector2d& one, const fvector2d& two) { return one.x > two.x && one.y > two.y; }

	friend bool operator<(const fvector2d& one, const fvector2d& two) { return one.x < two.x && one.y < two.y; }

	bool is_valid() {
		if (x == 0 || y == 0)
			return false;
		else
			return true;
	}

};

struct fvector {
	float x, y, z;

	fvector() : x(0.f), y(0.f), z(0.f) {}

	fvector(float x, float y, float z) : x(x), y(y), z(z) {}

	fvector(float InF) : x(InF), y(InF), z(InF) { }

	float Size() const { return sqrtf(x * x + y * y + z * z); }

	float Sum() const { return x + y + z; }

	float Size2D() const { return sqrtf(x * x + y * y); }

	float SizeSquared() const { return x * x + y * y + z * z; }

	float DistTo(const fvector& V) const { return (*this - V).Size(); }

	fvector operator+(const fvector& other) const { return fvector(x + other.x, y + other.y, z + other.z); }

	fvector operator-(const fvector& other) const { return fvector(x - other.x, y - other.y, z - other.z); }

	fvector operator*(const fvector& V) const { return fvector(x * V.x, y * V.y, z * V.z); }

	fvector operator/(const fvector& V) const { return fvector(x / V.x, y / V.y, z / V.z); }

	bool operator==(const fvector& V) const { return x == V.x && y == V.y && z == V.z; }

	bool operator!=(const fvector& V) const { return x != V.x || y != V.y || z != V.z; }

	fvector operator-() const { return fvector(-x, -y, -z); }

	fvector operator+(float Bias) const { return fvector(x + Bias, y + Bias, z + Bias); }

	fvector operator-(float Bias) const { return fvector(x - Bias, y - Bias, z - Bias); }

	fvector operator*(float Scale) const { return fvector(x * Scale, y * Scale, z * Scale); }

	fvector operator/(float Scale) const { const float RScale = 1.f / Scale; return fvector(x * RScale, y * RScale, z * RScale); }

	fvector operator=(const fvector& V) { x = V.x; y = V.y; z = V.z; return *this; }

	fvector operator+=(const fvector& V) { x += V.x; y += V.y; z += V.z; return *this; }

	fvector operator-=(const fvector& V) { x -= V.x; y -= V.y; z -= V.z; return *this; }

	fvector operator*=(const fvector& V) { x *= V.x; y *= V.y; z *= V.z; return *this; }

	fvector operator/=(const fvector& V) { x /= V.x; y /= V.y; z /= V.z; return *this; }

	fvector operator*=(float Scale) { x *= Scale; y *= Scale; z *= Scale; return *this; }

	fvector operator/=(float V) { const float RV = 1.f / V; x *= RV; y *= RV; z *= RV; return *this; }

	float operator|(const fvector& V) const { return x * V.x + y * V.y + z * V.z; }

	fvector operator^(const fvector& V) const { return fvector(y * V.z - z * V.y, z * V.x - x * V.z, x * V.y - y * V.x); }

	float Dot(fvector v) {
		return (x * v.x) + (y * v.y) + (z * v.z);
	}

	inline float Distance(fvector v) {
		return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	inline fvector Clamp() {
		fvector clamped = { x, y, 0 };
		while (clamped.y < -180.0f)
			clamped.y += 360.0f;
		while (clamped.y > 180.0f)
			clamped.y -= 360.0f;
		while (clamped.x < -180.0f)
			clamped.x += 360.0f;
		while (clamped.x > 180.0f)
			clamped.x -= 360.0f;
		return clamped;
	}

	bool is_valid() {
		if (x == 0 || y == 0)
			return false;
		else
			return true;
	}

	static const fvector ZeroVector;

	static const fvector OneVector;
};

struct alignas(16) FPlane : public fvector {
	float W;
};

enum class EAresAlliance : uint8_t {
	Alliance_Ally = 0,
	Alliance_Enemy = 1,
	Alliance_Neutral = 2,
	Alliance_Any = 3,
	Alliance_Count = 4,
	Alliance_MAX = 5
};

struct fmatrix {
	struct FPlane XPlane;
	struct FPlane YPlane;
	struct FPlane ZPlane;
	struct FPlane WPlane;
};

struct flinearcolor {
	float r, g, b, a;
	flinearcolor() : r(0), g(0), b(0), a(0) { }
	flinearcolor(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) { }
	flinearcolor(float r, float g, float b) : r(r), g(g), b(b), a(1.f) {}
};

template <class type>
struct tarray {
public:
	tarray() { this->data = nullptr; this->count = this->max = 0; };
	tarray(type* data, int32_t count) { this->data = data; this->count = this->max = count; }

	type* data;
	int32_t count, max;

	type& operator[](int i) {
		return this->data[i];
	};

	int size() {
		return this->count;
	}

	bool valid(int i) {
		return bool(i < this->count);
	}
};

class fstring : public tarray<wchar_t>
{
public:
	inline fstring() { };

	fstring(const wchar_t* other) {
		max = count = *other ? static_cast<int>(wcslen((wchar_t*)other)) + 1 : 0;

		if (count) {
			data = const_cast<wchar_t*>(other);
		}
	};
	fstring(const wchar_t* other, int count) {
		data = const_cast<wchar_t*>(other);;
		max = count = count;
	};

	inline bool is_valid() const {
		return data != nullptr;
	}

	inline const wchar_t* wide() const {
		return data;
	}
};

template<class tenum>
class tenumasbyte
{
public:
	inline tenumasbyte()
	{
	}

	inline tenumasbyte(tenum _value)
		: value(static_cast<uint8_t>(_value))
	{
	}

	explicit inline tenumasbyte(int32_t _value)
		: value(static_cast<uint8_t>(_value))
	{
	}

	explicit inline tenumasbyte(uint8_t _value)
		: value(_value)
	{
	}

	inline operator tenum() const
	{
		return (tenum)value;
	}

	inline tenum GetValue() const
	{
		return (tenum)value;
	}

private:
	uint8_t value;
};

enum class etracetypequery : uint8_t {
	tracetypequery1 = 0,
	tracetypequery2 = 1,
	tracetypequery3 = 2,
	tracetypequery4 = 3,
	tracetypequery5 = 4,
	tracetypequery6 = 5,
	tracetypequery7 = 6,
	tracetypequery8 = 7,
	tracetypequery9 = 8,
	tracetypequery10 = 9,
	tracetypequery11 = 10,
	tracetypequery12 = 11,
	tracetypequery13 = 12,
	tracetypequery14 = 13,
	tracetypequery15 = 14,
	tracetypequery16 = 15,
	tracetypequery17 = 16,
	tracetypequery18 = 17,
	tracetypequery19 = 18,
	tracetypequery20 = 19,
	tracetypequery21 = 20,
	tracetypequery22 = 21,
	tracetypequery23 = 22,
	tracetypequery24 = 23,
	tracetypequery25 = 24,
	tracetypequery26 = 25,
	tracetypequery27 = 26,
	tracetypequery28 = 27,
	tracetypequery29 = 28,
	tracetypequery30 = 29,
	tracetypequery31 = 30,
	tracetypequery32 = 31,
	tracetypequery_max = 32,
	etracetypequery_max = 33
};

enum class edrawdebugtrace : uint8_t {
	edrawdebugtrace__none = 0,
	edrawdebugtrace__foroneframe = 1,
	edrawdebugtrace__forduration = 2,
	edrawdebugtrace__persistent = 3,
	edrawdebugtrace__edrawdebugtrace_max = 4
};

struct fhitresult
{
	unsigned char                                      bBlockingHit : 1;                                         // 0x0000(0x0001)
	unsigned char                                      bStartPenetrating : 1;                                    // 0x0000(0x0001)
	unsigned char                                      UnknownData00[0x3];                                       // 0x0001(0x0003) MISSED OFFSET
	float                                              Time;                                                     // 0x0004(0x0004) (ZeroConstructor, IsPlainOldData)
	float                                              Distance;                                                 // 0x0008(0x0004) (ZeroConstructor, IsPlainOldData)                                            // 0x0048(0x000C)
	float                                              PenetrationDepth;                                         // 0x0054(0x0004) (ZeroConstructor, IsPlainOldData)
	int                                                Item;                                                     // 0x0058(0x0004) (ZeroConstructor, IsPlainOldData)                                               // 0x006C(0x0008) (ExportObject, ZeroConstructor, InstancedReference, IsPlainOldData)
	unsigned char                                      UnknownData01[0x4];                                       // 0x0074(0x0004) MISSED OFFSET                                                // 0x0078(0x0008) (ZeroConstructor, IsPlainOldData)
	int                                                FaceIndex;                                                // 0x0080(0x0004) (ZeroConstructor, IsPlainOldData)
	unsigned char                                      UnknownData02[0x4];                                       // 0x0084(0x0004) MISSED OFFSET
	unsigned char                                      UnknownData066[0xFF];                                       // 0x0084(0x0004) MISSED OFFSET
};

struct matrix {
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;

		};
		float m[4][4];
	};
};

matrix get_matrix(fvector rot, fvector origin = fvector(0, 0, 0))
{
	float radPitch = (rot.x * float(M_PI) / 180.f);
	float radYaw = (rot.y * float(M_PI) / 180.f);
	float radRoll = (rot.z * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	matrix matrix;
	matrix._11 = CP * CY;
	matrix._12 = CP * SY;
	matrix._13 = SP;
	matrix._14 = 0.f;

	matrix._21 = SR * SP * CY - CR * SY;
	matrix._22 = SR * SP * SY + CR * CY;
	matrix._23 = -SR * CP;
	matrix._24 = 0.f;

	matrix._31 = -(CR * SP * CY + SR * SY);
	matrix._32 = CY * SR - CR * SP * SY;
	matrix._33 = CR * CP;
	matrix._34 = 0.f;

	matrix._41 = origin.x;
	matrix._42 = origin.y;
	matrix._43 = origin.z;
	matrix._44 = 1.f;

	return matrix;
}

struct camera
{
	camera(fvector loc, fvector rot, float fov) : loc(loc), rot(rot), fov(fov) {}

	fvector loc, rot;
	float fov;

	fvector2d w2s(fvector world_loc)
	{
		fvector2d Screenlocation = fvector2d(0, 0);

		matrix tempMatrix = get_matrix(rot); // Matrix

		fvector vAxisX, vAxisY, vAxisZ;

		vAxisX = fvector(tempMatrix._11, tempMatrix._12, tempMatrix._13);
		vAxisY = fvector(tempMatrix._21, tempMatrix._22, tempMatrix._23);
		vAxisZ = fvector(tempMatrix._31, tempMatrix._32, tempMatrix._33);

		fvector vDelta = world_loc - loc;
		fvector vTransformed = fvector(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

		if (vTransformed.z < 1.f)
			vTransformed.z = 1.f;

		float FovAngle = fov;
		float ScreenCenterX = width / 2.f;
		float ScreenCenterY = height / 2.f;

		Screenlocation.x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;
		Screenlocation.y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;

		return Screenlocation;
	}

	bool w2s(fvector world_loc, fvector2d* out)
	{
		matrix tempMatrix = get_matrix(rot); // Matrix

		fvector vAxisX, vAxisY, vAxisZ;

		vAxisX = fvector(tempMatrix._11, tempMatrix._12, tempMatrix._13);
		vAxisY = fvector(tempMatrix._21, tempMatrix._22, tempMatrix._23);
		vAxisZ = fvector(tempMatrix._31, tempMatrix._32, tempMatrix._33);

		fvector vDelta = world_loc - loc;
		fvector vTransformed = fvector(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

		if (vTransformed.z < 1.f)
			vTransformed.z = 1.f;

		float FovAngle = fov;
		float ScreenCenterX = width / 2.f;
		float ScreenCenterY = height / 2.f;

		out->x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;
		out->y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;

		return out->x > 0 && out->x < width && out->y > 0 && out->y < height;
	}
};