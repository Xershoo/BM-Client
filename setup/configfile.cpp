#include "configfile.h"

ConfigFile::ConfigFile()
    :m_settings(NULL),m_filePath(""),m_exists(false)
{

}

ConfigFile::~ConfigFile()
{
    if(m_settings)
    {
        delete m_settings;
        m_settings = NULL;
    }
}
