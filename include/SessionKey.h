#pragma once
#include <cstdlib>
#include <ctime>
#include <math.h>

#include <QtGlobal>
#include <QString>

#include <gmpxx.h>

class SessionKey
{
public:
    SessionKey();
    int getIntermediateNumberForServer();
    void calculateSessionKey( const int& intermadiate_number_from_server );
    int getSessionKey();

private:
    int randRange( int low,int high );
    mpz_t p_mpz_mod_;
    mpz_t g_mpz_;
    int random_number_;
    int intermediate_number_;
    mpz_t intermediate_number_mpz_;
    mpz_t session_key_mpz_;
    int session_key_;
};
