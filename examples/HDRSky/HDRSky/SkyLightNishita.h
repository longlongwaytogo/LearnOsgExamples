#ifndef _SKY_LIGHT_NISHITA_H_
#define _SKY_LIGHT_NISHITA_H_

#pragma once

#include <vector>
#include <assert.h>
#include <osg/Vec3>
#include <osg/Vec3d>
#include <osg/Vec4>


typedef double f64;
typedef float f32;
typedef unsigned int uint32;
typedef unsigned int uint;
typedef int int32;


#if defined(USE_FLOAT)
	typedef float float_sky;
#else
	typedef f64 float_sky;
#endif
#define _ALIGN(a) 
#define ILINE inline

#include <osg/referenced>

template<class T> ILINE T clamp_tpl( T X, T Min, T Max ) 
{	
	return X<Min ? Min : X<Max ? X : Max; 
}

class SkyLightManager;

class SkyLightNishita : public osg::Referenced
{
private:

	// size of lookup tables
	static const uint cOLUT_HeightSteps = 32;
	static const uint cOLUT_AngularSteps = 256;

	// definition of optical depth LUT for mie/rayleigh scattering 
	struct SOpticalDepthLUTEntry
	{
		f32 mie;
		f32 rayleigh;	 
	};

	// definition of optical scale LUT for mie/rayleigh scattering 
	struct SOpticalScaleLUTEntry
	{
		f32 atmosphereLayerHeight;
		f32 mie;
		f32 rayleigh;
	};

	// definition lookup table entry for phase function
	struct SPhaseLUTEntry
	{
		f32 mie;
		f32 rayleigh;
	};

	// definition of lookup tables
	typedef std::vector< SOpticalDepthLUTEntry > OpticalDepthLUT;
	typedef std::vector< SOpticalScaleLUTEntry > OpticalScaleLUT;

	static const unsigned int cPLUT_AngularSteps = 256;
	//replace std::vector by a custom class with static storage
	class PhaseLUT
	{
		SPhaseLUTEntry m_LUT[cPLUT_AngularSteps] _ALIGN(128);
		size_t m_Size;
	public:
		PhaseLUT() : m_Size(0) { }
		SPhaseLUTEntry &operator[] (size_t index) { return m_LUT[index]; }
		const SPhaseLUTEntry &operator[] (size_t index) const { return m_LUT[index]; }
		void resize(size_t size) { m_Size = size; };
		void reserve(size_t) { }
		void push_back(SPhaseLUTEntry &entry) { m_LUT[m_Size++] = entry; }
		size_t size() const { return m_Size; }
	};

public:
	SkyLightNishita();
	~SkyLightNishita();

	// set sky dome conditions
	void SetAtmosphericConditions( const osg::Vec3& sunIntensity, const f32 Km, const f32 Kr, const f32 g );
	void SetRGBWaveLengths( const osg::Vec3& rgbWaveLengths );
	void SetSunDirection( const osg::Vec3& sunDir );
	
	// compute sky colors



	void ComputeSkyColor( const osg::Vec3& skyDir, osg::Vec3* pInScattering, osg::Vec3* pInScatteringMieNoPremul, osg::Vec3* pInScatteringRayleighNoPremul, osg::Vec3* pInScatteringRayleigh ) const;

	void SetInScatteringIntegralStepSize( int stepSize );
	int GetInScatteringIntegralStepSize() const;

	// constants for final pixel shader processing, if "no pre-multiplied in-scattering " colors are to be processed in a pixel shader
	osg::Vec4 GetPartialMieInScatteringConst() const;
	osg::Vec4 GetPartialRayleighInScatteringConst() const;
	osg::Vec3 GetSunDirection() const;
	osg::Vec4 GetPhaseFunctionConsts() const;

private:
	// mapping helpers
	f64 MapIndexToHeight( uint index ) const;
	f64 MapIndexToCosVertAngle( uint index ) const;
	f32 MapIndexToCosPhaseAngle( uint index ) const;

	void MapCosPhaseAngleToIndex( const f32 cosPhaseAngle, uint& index, f32& indexFrc ) const;

	// optical lookup table access helpers
	uint OpticalLUTIndex( uint heightIndex, uint cosVertAngleIndex ) const;

	// computes lookup tables for optical depth, etc.
	void ComputeOpticalLUTs();

	// computes lookup table for phase function
	void ComputePhaseLUT();

	// computes optical depth (helpers for ComputeOpticalLUTs())
	f64 IntegrateOpticalDepth( const osg::Vec3d& start, const osg::Vec3d& end, 
		const f64& avgDensityHeightInv, const f64& error ) const;

	bool ComputeOpticalDepth( const osg::Vec3d& cameraLookDir, const f64& cameraHeight, const f64& avgDensityHeightInv, float& depth ) const;

	// does a bilinearily filtered lookup into the optical depth LUT
	// SOpticalDepthLUTEntry* is passed to save address resolve operations
	ILINE SOpticalDepthLUTEntry LookupBilerpedOpticalDepthLUTEntry( const SOpticalDepthLUTEntry* const __restrict cpOptDepthLUT, 
		uint heightIndex, const f32 cosVertAngle ) const;

	// does a bilinearily filtered lookup into the phase LUT
	SPhaseLUTEntry LookupBilerpedPhaseLUTEntry( const f32 cosPhaseAngle ) const;

	// computes in-scattering
#if defined(SKYLIGHT_USE_VMX)
	// void SamplePartialInScatteringAtHeight( const SOpticalScaleLUTEntry& osAtHeight, 
		// const __vector4 outScatteringConstMie, const __vector4 outScatteringConstRayleigh, const SOpticalDepthLUTEntry& odAtHeightSky, 
		// const SOpticalDepthLUTEntry& odAtViewerSky, const SOpticalDepthLUTEntry& odAtHeightSun, 
		// osg::Vec3& partialInScatteringMie, osg::Vec3& partialInScatteringRayleigh ) const;
#else
	void SamplePartialInScatteringAtHeight( const SOpticalScaleLUTEntry& osAtHeight, 
		const f32 outScatteringConstMie, const osg::Vec3& outScatteringConstRayleigh, const SOpticalDepthLUTEntry& odAtHeightSky, 
		const SOpticalDepthLUTEntry& odAtViewerSky, const SOpticalDepthLUTEntry& odAtHeightSun, 
		osg::Vec3& partialInScatteringMie, osg::Vec3& partialInScatteringRayleigh ) const;
#endif





#if defined(SKYLIGHT_USE_VMX)
	// void ComputeInScatteringNoPremul( const __vector4 outScatteringConstMie, const __vector4 outScatteringConstRayleigh, const osg::Vec3& skyDir,
		// osg::Vec3& inScatteringMieNoPremul, osg::Vec3& inScatteringRayleighNoPremul ) const;
#else
	void ComputeInScatteringNoPremul( const f32 outScatteringConstMie, const osg::Vec3& outScatteringConstRayleigh, const osg::Vec3& skyDir,
		osg::Vec3& inScatteringMieNoPremul, osg::Vec3& inScatteringRayleighNoPremul ) const;
#endif
	// serialization of optical LUTs
	bool LoadOpticalLUTs();
	void SaveOpticalLUTs() const;

	const OpticalScaleLUT& GetOpticalScaleLUT() const;

private:
	// lookup tables
	OpticalDepthLUT m_opticalDepthLUT;
	OpticalScaleLUT m_opticalScaleLUT;
	PhaseLUT m_phaseLUT;

	// mie scattering constant
	f32 m_Km; 

	// rayleigh scattering constant
	f32 m_Kr; 

	// sun intensity
	osg::Vec3 m_sunIntensity;

	// mie scattering asymmetry factor (g is always 0.0 for rayleigh scattering)
	f32 m_g;

	// wavelengths for r, g, and b to the -4th used for mie rayleigh scattering
	osg::Vec3 m_invRGBWaveLength4;

	// direction towards the sun
	osg::Vec3 m_sunDir;

	// step size for solving in-scattering integral
	int m_inScatteringStepSize;

	friend class SkyLightManager;
};

ILINE void SkyLightNishita::SetRGBWaveLengths( const osg::Vec3& rgbWaveLengths )
{
	assert( 380.0f <= rgbWaveLengths.x() && 780.0f >= rgbWaveLengths.x() );
	assert( 380.0f <= rgbWaveLengths.y() && 780.0f >= rgbWaveLengths.y() );
	assert( 380.0f <= rgbWaveLengths.z() && 780.0f >= rgbWaveLengths.z() );

	m_invRGBWaveLength4.x() = powf( rgbWaveLengths.x() * 1e-3f, -4.0f );
	m_invRGBWaveLength4.y() = powf( rgbWaveLengths.y() * 1e-3f, -4.0f );
	m_invRGBWaveLength4.z() = powf( rgbWaveLengths.z() * 1e-3f, -4.0f );
}

ILINE void SkyLightNishita::SetSunDirection( const osg::Vec3& sunDir )
{
	assert( sunDir.length2() > 0.0f );
	m_sunDir = sunDir;
	m_sunDir.normalize();
}

ILINE void SkyLightNishita::SetAtmosphericConditions( const osg::Vec3& sunIntensity, 
																								const f32 Km, const f32 Kr, const f32 g )
{
	m_sunIntensity = sunIntensity;
	m_Km = Km;
	m_Kr = Kr;

	// update g only if it changed as phase lut needs to be rebuilt
	float newg(clamp_tpl(g, -0.9995f, 0.9995f));
	if( fabsf( m_g - newg ) > 1e-6f )
	{
		m_g = newg;
		ComputePhaseLUT();
	}
}

ILINE const SkyLightNishita::OpticalScaleLUT& SkyLightNishita::GetOpticalScaleLUT() const
{
	return m_opticalScaleLUT;
}

#endif // #ifndef _SKY_LIGHT_NISHITA_H_
