/**************************************************************************
 *
 * Copyright 2014 Valve Software
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/

#include "glv_settings.h"

// ------------------------------------------------------------------------------------------------
void glv_SettingInfo_print(const glv_SettingInfo* pSetting)
{
    if (pSetting->bPrintInHelp)
    {
        char * pStrParams;
        if (pSetting->type == GLV_SETTING_STRING)
        {
            pStrParams = "<string>";
        } else if (pSetting->type == GLV_SETTING_BOOL) {
            pStrParams = "<BOOL>  ";
        } else if (pSetting->type == GLV_SETTING_UINT) {
            pStrParams = "<uint>  ";
        } else if (pSetting->type == GLV_SETTING_INT) {
            pStrParams = "<int>   ";
        } else {
            pStrParams = "< ??? > ";
        }
        printf("    -%s,--%s %s\t: %s\n", pSetting->pShortName, pSetting->pLongName, pStrParams, pSetting->pDesc);
    }
}

// ------------------------------------------------------------------------------------------------
void glv_SettingGroup_print(const glv_SettingGroup* pSettingGroup)
{
    unsigned int i;
    printf("%s available options:\n", pSettingGroup->pName);

    for (i = 0; i < pSettingGroup->numSettings; i++)
    {
        glv_SettingInfo_print(&(pSettingGroup->pSettings[i]));
    }
}

// ------------------------------------------------------------------------------------------------
BOOL glv_SettingInfo_parse_value(glv_SettingInfo* pSetting, const char* arg)
{
    switch(pSetting->type)
    {
    case GLV_SETTING_STRING:
        {
            glv_free(pSetting->pType_data);
            pSetting->pType_data = glv_allocate_and_copy(arg);
        }
        break;
    case GLV_SETTING_BOOL:
        {
            BOOL bTrue = FALSE;
            bTrue = strncmp(arg, "true", 4) == 0 || strncmp(arg, "TRUE", 4) == 0 || strncmp(arg, "True", 4) == 0;
            *(BOOL*)pSetting->pType_data = bTrue;
        }
        break;
    case GLV_SETTING_UINT:
        {
            if (sscanf(arg, "%u", (unsigned int*)pSetting->pType_data) != 1)
            {
                printf("Invalid unsigned int setting: '%s'\n", arg);
                return FALSE;
            }
        }
        break;
    case GLV_SETTING_INT:
        {
            if (sscanf(arg, "%d", (int*)pSetting->pType_data) != 1)
            {
                printf("Invalid int setting: '%s'\n", arg);
                return FALSE;
            }
        }
        break;
    default:
        assert(!"Unhandled setting type");
        return FALSE;
    }

    return TRUE;
}

// ------------------------------------------------------------------------------------------------
char* glv_SettingInfo_stringify_value(glv_SettingInfo* pSetting)
{
    switch(pSetting->type)
    {
    case GLV_SETTING_STRING:
        {
            return glv_allocate_and_copy((const char*)pSetting->pType_data);
        }
        break;
    case GLV_SETTING_BOOL:
        {
            return (*(BOOL*)pSetting->pType_data ? glv_allocate_and_copy("TRUE") : glv_allocate_and_copy("FALSE"));
        }
        break;
    case GLV_SETTING_UINT:
        {
            char value[100];
            memset(value, 0, 100);
            sprintf(value, "%u", *(unsigned int*)pSetting->pType_data);
            return glv_allocate_and_copy(value);
        }
        break;
    case GLV_SETTING_INT:
        {
            char value[100];
            memset(value, 0, 100);
            sprintf(value, "%d", *(int*)pSetting->pType_data);
            return glv_allocate_and_copy(value);
        }
        break;
    default:
        assert(!"Unhandled setting type");
        break;
    }
    return glv_allocate_and_copy("<unhandled setting type>");
}

// ------------------------------------------------------------------------------------------------
void glv_SettingGroup_reset_defaults(glv_SettingGroup* pSettingGroup)
{
    if (pSettingGroup != NULL)
    {
        unsigned int u;
        for (u = 0; u < pSettingGroup->numSettings; u++)
        {
            glv_SettingInfo_reset_default(&pSettingGroup->pSettings[u]);
        }
    }
}

// ------------------------------------------------------------------------------------------------
void glv_SettingInfo_reset_default(glv_SettingInfo* pSetting)
{
    assert(pSetting != NULL);
    switch(pSetting->type)
    {
    case GLV_SETTING_STRING:
        if (pSetting->pType_data != NULL)
        {
            glv_free(pSetting->pType_data);
        }

        if (pSetting->pType_default == NULL)
        {
            pSetting->pType_data = NULL;
        }
        else
        {
            pSetting->pType_data = glv_allocate_and_copy(*((const char**)pSetting->pType_default));
        }
        break;
    case GLV_SETTING_BOOL:
        *(BOOL*)pSetting->pType_data = *(BOOL*)pSetting->pType_default;
        break;
    case GLV_SETTING_UINT:
        *(unsigned int*)pSetting->pType_data = *(unsigned int*)pSetting->pType_default;
        break;
    case GLV_SETTING_INT:
        *(int*)pSetting->pType_data = *(int*)pSetting->pType_default;
        break;
    default:
        assert(!"Unhandled GLV_SETTING_TYPE");
        break;
    }
}

// ------------------------------------------------------------------------------------------------
int glv_SettingGroup_Load_from_file(FILE* pFile, glv_SettingGroup** ppSettingGroups, unsigned int* pNumSettingGroups)
{
    int retVal = 0;
    char* line = GLV_NEW_ARRAY(char, 1024);

    assert(pFile != NULL);
    assert(ppSettingGroups != NULL);
    assert(pNumSettingGroups != NULL);
    *pNumSettingGroups = 0;

    if (line == NULL)
    {
        printf("Out of memory while reading settings file\n");
        retVal = -1;
    }
    else
    {
        glv_SettingGroup* pCurGroup = NULL;
        while (feof(pFile) == 0 && ferror(pFile) == 0)
        {
            char* lineStart;
            char* pOpenBracket;
            char* pCloseBracket;
            line = fgets(line, 1024, pFile);
            if (line == NULL)
            {
                break;
            }

            // if line ends with a newline, then replace it with a NULL
            if (line[strlen(line)-1] == '\n')
            {
                line[strlen(line)-1] = '\0';
            }

            // remove any leading whitespace
            lineStart = line;
            while (*lineStart == ' ') { ++lineStart; }

            // skip empty lines
            if (strlen(lineStart) == 0)
            {
                continue;
            }

            // if the line starts with "#" or "//", then consider it a comment and ignore it.
            // if the first 'word' is only "-- " then the remainder of the line is for application arguments
            // else first 'word' in line should be a long setting name and the rest of line is value for setting
            if (lineStart[0] == '#' || (lineStart[0] == '/' && lineStart[1] == '/'))
            {
                // its a comment, continue to next loop iteration
                continue;
            }

            pOpenBracket = strchr(lineStart, '[');
            pCloseBracket = strchr(lineStart, ']');
            if (pOpenBracket != NULL && pCloseBracket != NULL)
            {
                // a group was found!
                unsigned int i;
                char* pGroupName = glv_allocate_and_copy_n(pOpenBracket + 1, pCloseBracket - pOpenBracket - 1);

                // Check to see if we already have this group
                pCurGroup = NULL;
                for (i = 0; i < *pNumSettingGroups; i++)
                {
                    if (strcmp((*ppSettingGroups)[i].pName, pGroupName) == 0)
                    {
                        // we already have this group!
                        pCurGroup = &(*ppSettingGroups)[i];
                        break;
                    }
                }

                if (pCurGroup == NULL)
                {
                    // Need to grow our list of groups!
                    glv_SettingGroup* pTmp = *ppSettingGroups;
                    unsigned int lastIndex = *pNumSettingGroups;

                    (*pNumSettingGroups) += 1;

                    *ppSettingGroups = GLV_NEW_ARRAY(glv_SettingGroup, *pNumSettingGroups);
                    memcpy(*ppSettingGroups, pTmp, lastIndex * sizeof(glv_SettingGroup));

                    pCurGroup = &(*ppSettingGroups)[lastIndex];
                    pCurGroup->pName = pGroupName;
                }
            }
            else
            {
                char* pTokName = strtok(lineStart, "=");
                char* pTokValue = strtok(NULL, "=");
                if (pTokName != NULL && pTokValue != NULL)
                {
                    // A setting name and value were found!
                    char* pValueStart = pTokValue;
                    char* pTmpEndName = pTokName;

                    assert(pCurGroup != NULL);
                    if (pCurGroup != NULL)
                    {
                        // create a SettingInfo to store this information
                        glv_SettingInfo info;
                        glv_SettingInfo* pTmp;
                        memset(&info, 0, sizeof(glv_SettingInfo));

                        // trim trailing whitespace by turning it into a null char
                        while (*pTmpEndName != '\0')
                        {
                            if (*pTmpEndName == ' ')
                            {
                                *pTmpEndName = '\0';
                                break;
                            }
                            else
                            {
                                ++pTmpEndName;
                            }
                        }

                        info.pLongName = glv_allocate_and_copy(pTokName);
                        info.type = GLV_SETTING_STRING;

                        // remove leading whitespace from value
                        while (*pValueStart == ' ') { ++pValueStart; }

                        info.pType_data = glv_allocate_and_copy(pValueStart);

                        // add it to the current group
                        pTmp = pCurGroup->pSettings;
                        pCurGroup->numSettings += 1;
                        pCurGroup->pSettings = GLV_NEW_ARRAY(glv_SettingInfo, pCurGroup->numSettings);
                        if (pTmp != NULL)
                        {
                            memcpy(pCurGroup->pSettings, pTmp, pCurGroup->numSettings * sizeof(glv_SettingInfo));
                        }
                        pCurGroup->pSettings[pCurGroup->numSettings - 1] = info;
                    }
                }
                else
                {
                    printf("Could not parse a line in settings file: '%s'\n", line);
                }
            }
        }
    }

    GLV_DELETE(line);

    return retVal;
}

// ------------------------------------------------------------------------------------------------
void glv_SettingGroup_Delete_Loaded(glv_SettingGroup** ppSettingGroups, unsigned int* pNumSettingGroups)
{
    unsigned int g;
    unsigned int s;
    assert(ppSettingGroups != NULL);
    assert(*ppSettingGroups != NULL);
    assert(pNumSettingGroups != NULL);

    for (g = 0; g < *pNumSettingGroups; g++)
    {
        glv_SettingGroup* pGroup = &(*ppSettingGroups)[g];
        glv_free((void*)pGroup->pName);
        pGroup->pName = NULL;

        for (s = 0; s < pGroup->numSettings; s++)
        {
            glv_free((void*)pGroup->pSettings[s].pLongName);
            pGroup->pSettings[s].pLongName = NULL;
            glv_free(pGroup->pSettings[s].pType_data);
            pGroup->pSettings[s].pType_data = NULL;
        }

        GLV_DELETE(pGroup->pSettings);
        pGroup->pSettings = NULL;
    }

    GLV_DELETE(*ppSettingGroups);
    *pNumSettingGroups = 0;
}

// ------------------------------------------------------------------------------------------------
void glv_SettingGroup_Apply_Overrides(glv_SettingGroup* pSettingGroup, glv_SettingGroup* pOverrideGroups, unsigned int numOverrideGroups)
{
    unsigned int g;
    assert(pSettingGroup != NULL);
    assert(pOverrideGroups != NULL);

    // only override matching group (based on name)
    for (g = 0; g < numOverrideGroups; g++)
    {
        if (strcmp(pSettingGroup->pName, pOverrideGroups[g].pName) == 0)
        {
            unsigned int s;
            glv_SettingGroup* pOverride = &pOverrideGroups[g];

            for (s = 0; s < pOverride->numSettings; s++)
            {
                // override matching settings based on long name
                if (strcmp(pSettingGroup->pSettings[s].pLongName, pOverride->pSettings[s].pLongName) == 0)
                {
                    char* pTmp = glv_SettingInfo_stringify_value(&pOverride->pSettings[s]);
                    if (glv_SettingInfo_parse_value(&pSettingGroup->pSettings[s], pTmp) == FALSE)
                    {
                        assert(!"Failed to parse override value");
                    }
                    glv_free(pTmp);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
BOOL glv_SettingGroup_save(glv_SettingGroup* pSettingGroup, unsigned int numSettingGroups, FILE* pSettingsFile)
{
    BOOL retVal = TRUE;

    if (pSettingGroup == NULL)
    {
        glv_LogError("Cannot save a null group of settings.\n");
        retVal = FALSE;
    }

    if (pSettingsFile == NULL)
    {
        glv_LogError("Cannot save an unnamed settings file.\n");
        retVal = FALSE;
    }

    if (retVal == TRUE)
    {
        unsigned int g;
        unsigned int index;

        for (g = 0; g < numSettingGroups; g++)
        {
            // group name
            fputs("[", pSettingsFile);
            fputs(pSettingGroup[g].pName, pSettingsFile);
            fputs("]\n", pSettingsFile);

            // settings
            for (index = 0; index < pSettingGroup[g].numSettings; index++)
            {
                char* value = NULL;
                fputs("   ", pSettingsFile);
                fputs(pSettingGroup[g].pSettings[index].pLongName, pSettingsFile);
                fputs(" = ", pSettingsFile);
                value = glv_SettingInfo_stringify_value(&pSettingGroup[g].pSettings[index]);
                fputs(value, pSettingsFile);
                glv_free(value);
                fputs("\n", pSettingsFile);
            }

            fputs("\n", pSettingsFile);
        }
    }

    return retVal;
}

//-----------------------------------------------------------------------------
int glv_SettingGroup_init_from_cmdline(glv_SettingGroup* pSettingGroup, int argc, char* argv[], char** ppOut_remaining_args)
{
    int i = 0;

    if (pSettingGroup != NULL)
    {
        glv_SettingInfo* pSettings = pSettingGroup->pSettings;
        unsigned int num_settings = pSettingGroup->numSettings;

        // update settings based on command line options
        for (i = 1; i < argc; )
        {
            unsigned int settingIndex;
            int consumed = 0;
            char* curArg = argv[i];

            // if the arg is only "--" then all following args are for the application;
            // if the arg starts with "-" then it is referring to a short name;
            // if the arg starts with "--" then it is referring to a long name.
            if (strcmp("--", curArg) == 0 && ppOut_remaining_args != NULL)
            {
                // all remaining args are for the application

                // increment past the current arg
                i += 1;
                consumed++;
                for (; i < argc; i++)
                {
                    if (*ppOut_remaining_args == NULL || strlen(*ppOut_remaining_args) == 0)
                    {
                        *ppOut_remaining_args = glv_allocate_and_copy(argv[i]);
                    }
                    else
                    {
                        *ppOut_remaining_args = glv_copy_and_append(*ppOut_remaining_args, " ", argv[i]);
                    }
                    consumed++;
                }
            }
            else
            {
                for (settingIndex = 0; settingIndex < num_settings; settingIndex++)
                {
                    const char* pSettingName = NULL;
                    curArg = argv[i];
                    if (strncmp("--", curArg, 2) == 0)
                    {
                        // long option name
                        pSettingName = pSettings[settingIndex].pLongName;
                        curArg += 2;
                    }
                    else if (strncmp("-", curArg, 1) == 0)
                    {
                        // short option name
                        pSettingName = pSettings[settingIndex].pShortName;
                        curArg += 1;
                    }

                    if (pSettingName != NULL && strcmp(curArg, pSettingName) == 0)
                    {
                        if (glv_SettingInfo_parse_value(&pSettings[settingIndex], argv[i+1]))
                        {
                            consumed += 2;
                        }
                        break;
                    }
                }
            }

            if (consumed == 0)
            {
                printf("Error: Invalid argument found '%s'\n", curArg);
                glv_SettingGroup_print(pSettingGroup);
                glv_SettingGroup_delete(pSettingGroup);
                return -1;
            }

            i += consumed;
        }
    }

    return 0;
}

// ------------------------------------------------------------------------------------------------
int glv_SettingGroup_init(glv_SettingGroup* pSettingGroup, FILE* pSettingsFile, int argc, char* argv[], char** ppOut_remaining_args)
{
    unsigned int u;
    unsigned int num_settings;
    glv_SettingInfo* pSettings;

    if (pSettingGroup == NULL)
    {
        assert(!"No need to call glv_SettingGroup_init if the application has no settings");
        return 0;
    }

    if (argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0))
    {
        glv_SettingGroup_print(pSettingGroup);
        return -1;
    }

    // Initially, set all options to their defaults
    glv_SettingGroup_reset_defaults(pSettingGroup);
    num_settings = pSettingGroup->numSettings;
    pSettings = pSettingGroup->pSettings;

    // Secondly set options based on settings file
    if (pSettingsFile != NULL)
    {
        glv_SettingGroup* pGroups = NULL;
        unsigned int numGroups = 0;
        if (glv_SettingGroup_Load_from_file(pSettingsFile, &pGroups, &numGroups) == -1)
        {
            glv_SettingGroup_print(pSettingGroup);
            return -1;
        }

        glv_SettingGroup_Apply_Overrides(pSettingGroup, pGroups, numGroups);

        glv_SettingGroup_Delete_Loaded(&pGroups, &numGroups);
    }

    // Thirdly set options based on cmd line args
    if (glv_SettingGroup_init_from_cmdline(pSettingGroup, argc, argv, ppOut_remaining_args) == -1)
    {
        glv_SettingGroup_print(pSettingGroup);
        return -1;
    }

    return 0;
}

// ------------------------------------------------------------------------------------------------
void glv_SettingGroup_delete(glv_SettingGroup* pSettingGroup)
{
    if (pSettingGroup != NULL)
    {
        unsigned int i;

        // need to delete all strings
        for (i = 0; i < pSettingGroup->numSettings; i++)
        {
            if (pSettingGroup->pSettings[i].type == GLV_SETTING_STRING)
            {
                if (pSettingGroup->pSettings[i].pType_data != NULL)
                {
                    glv_free(pSettingGroup->pSettings[i].pType_data);
                    pSettingGroup->pSettings[i].pType_data = NULL;
                }
            }
        }
    }
}
