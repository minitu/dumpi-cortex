#include <string.h>
#include "cortex/cortex.h"

cortex_dumpi_profile* cortex_undumpi_open(const char* fname) {
	cortex_dumpi_profile* profile = (cortex_dumpi_profile*)malloc(sizeof(cortex_dumpi_profile));
	profile->dumpi = undumpi_open(fname);
	if(profile->dumpi == NULL) {
		free(profile);
		return NULL;
	}
	memset(&(profile->cbacks),0,sizeof(profile->cbacks));
	cortex_set_callbacks(&(profile->cbacks));
	libundumpi_populate_callbacks(&(profile->cbacks), profile->callarr);
	return profile;
}

void cortex_undumpi_close(cortex_dumpi_profile* profile) {
	undumpi_close(profile->dumpi);
	free(profile);
}

int cortex_dumpi_start_stream_read(cortex_dumpi_profile *profile) {
	return dumpi_start_stream_read(profile->dumpi);
}

dumpi_header* cortex_undumpi_read_header(cortex_dumpi_profile* profile) {
	return undumpi_read_header(profile->dumpi);
}
 
int cortex_undumpi_read_single_call(cortex_dumpi_profile *profile,
			       struct libundumpi_cbpair *callarr,
			       void *userarg,
			       int *mpi_finalized) {
	int r = undumpi_read_single_call(profile->dumpi, profile->callarr, (void*)profile, mpi_finalized);
	// TODO check the list of pending operations in profile and execute the user-provided callbacks

	return r; // TODO we should not return r is there are pending operations in the queue, we should return 1
}

int cortex_undumpi_read_stream_full(cortex_dumpi_profile* profile,
                          const libundumpi_callbacks *callback,
                          void *userarg,
                          bool print_progress, // TODO non-supported yet
                          double percent_terminate) // TODO non-supported yet
{
	libundumpi_cbpair callarr[DUMPI_END_OF_STREAM];
	memset(&callarr, 0, sizeof(libundumpi_cbpair) * DUMPI_END_OF_STREAM);
	libundumpi_populate_callbacks(&(profile->cbacks), callarr);
	int mpi_finalized = 0;
	int active = 1;
	while(active) {
		active = cortex_undumpi_read_single_call(profile, callarr, (void*)profile, &mpi_finalized);
		// TODO check the list of pending operations in profile and execute the user-provided callbacks
	}
	return 1;
}

int cortex_undumpi_read_stream(cortex_dumpi_profile* profile,
                          const libundumpi_callbacks *callback,
                          void *userarg) 
{
	return cortex_undumpi_read_stream_full(profile,callback,userarg,false,-1);
}

dumpi_keyval_record* cortex_undumpi_read_keyval_record(cortex_dumpi_profile* profile)
{
	return undumpi_read_keyval_record(profile->dumpi);
}

dumpi_footer* cortex_undumpi_read_footer(cortex_dumpi_profile* profile)
{
	return undumpi_read_footer(profile->dumpi);
}

dumpi_sizeof cortex_undumpi_read_datatype_sizes(cortex_dumpi_profile *profile)
{
	return undumpi_read_datatype_sizes(profile->dumpi);
}
