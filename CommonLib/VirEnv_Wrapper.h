#pragma once


#ifdef __cplusplus
extern "C"{
#endif

	typedef struct VirEnvHelper VirEnvHelper;

	VirEnvHelper* newVirEnvHelper();

	void VirEnv_shutdown(VirEnvHelper* VirEnv_c);

	void VirEnv_initialization(VirEnvHelper* VirEnv_c, const char* configPath);

	void VirEnv_runStep(VirEnvHelper* VirEnv_c, double simTime);

#ifdef RS_DSPACE
	int VirEnv_getConfig(VirEnvHelper* VirEnv_c, char* name, char* valChar);
	
	int VirEnv_readConfig(VirEnvHelper* VirEnv_c, FILE *fp);
#endif

	int VirEnv_isVeryFirstStep(VirEnvHelper* VirEnv_c);
	
#ifdef __cplusplus
}
#endif