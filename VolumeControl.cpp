class Mixer
{
public:
    /*************************************************************************/
    bool init()
    {
        _nNumMixers = mixerGetNumDevs();
        _hMixer = NULL;
        ZeroMemory(&_mxcaps, sizeof(MIXERCAPS));

        if (_nNumMixers != 0)
        {
            if (mixerOpen(&_hMixer, 0, 0, 0, MIXER_OBJECTF_MIXER) != MMSYSERR_NOERROR)
            {
                return FALSE;
            }

            if (mixerGetDevCaps((UINT)_hMixer, &_mxcaps, sizeof(MIXERCAPS)) != MMSYSERR_NOERROR)
            {
                return FALSE;
            }
        }

        return _hMixer != NULL;
    }

    /*************************************************************************/
    bool close()
    {
        return mixerClose(_hMixer) == MMSYSERR_NOERROR;
    }

    /*************************************************************************/
    bool GetMuteControl()
    {
        MIXERLINE mixerLine;
        mixerLine.cbStruct = sizeof(MIXERLINE);
        mixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;

        if (mixerGetLineInfo((HMIXEROBJ)_hMixer,
                             &mixerLine,
                             MIXER_OBJECTF_HMIXER |
                             MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
        {
            return FALSE;
        }

        MIXERCONTROL mixerControl;
        MIXERLINECONTROLS mixerLineControl;

        mixerLineControl.cbStruct         = sizeof(MIXERLINECONTROLS);
        mixerLineControl.dwLineID         = mixerLine.dwLineID;
        mixerLineControl.dwControlType    = MIXERCONTROL_CONTROLTYPE_MUTE;
        mixerLineControl.cControls        = 1;
        mixerLineControl.cbmxctrl         = sizeof(MIXERCONTROL);
        mixerLineControl.pamxctrl         = &mixerControl;

        if (mixerGetLineControls((HMIXEROBJ)_hMixer,
                                 &mixerLineControl,
                                 MIXER_OBJECTF_HMIXER |
                                 MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR)
        {
            return FALSE;
        }

        _controlID = mixerControl.dwControlID;

        return TRUE;
    }

    /*************************************************************************/
    bool GetMuteValue(long* value)
    {

        MIXERCONTROLDETAILS_BOOLEAN mxcdMute;
        MIXERCONTROLDETAILS mixerControlDetails;

        mixerControlDetails.cbStruct          = sizeof(MIXERCONTROLDETAILS);
        mixerControlDetails.dwControlID       = _controlID;
        mixerControlDetails.cChannels         = 1;
        mixerControlDetails.cMultipleItems    = 0;
        mixerControlDetails.cbDetails         = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
        mixerControlDetails.paDetails         = &mxcdMute;

        if (mixerGetControlDetails((HMIXEROBJ)_hMixer,
                                   &mixerControlDetails,
                                   MIXER_OBJECTF_HMIXER
                                   | MIXER_GETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
        {
            return FALSE;
        }

        *value = mxcdMute.fValue;
        return TRUE;
    }

    bool GetMuteStatus(bool& value)
    {

        MIXERCONTROLDETAILS_BOOLEAN mxcdMute;
        MIXERCONTROLDETAILS mixerControlDetails;

        mixerControlDetails.cbStruct          = sizeof(MIXERCONTROLDETAILS);
        mixerControlDetails.dwControlID       = _controlID;
        mixerControlDetails.cChannels         = 1;
        mixerControlDetails.cMultipleItems    = 0;
        mixerControlDetails.cbDetails         = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
        mixerControlDetails.paDetails         = &mxcdMute;

        if (mixerGetControlDetails((HMIXEROBJ)_hMixer,
                                   &mixerControlDetails,
                                   MIXER_OBJECTF_HMIXER
                                   | MIXER_GETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
        {
            return FALSE;
        }

        value = mxcdMute.fValue;
        return TRUE;
    }


    /*************************************************************************/
    bool SetMute( bool state)
    {

        MIXERCONTROLDETAILS_BOOLEAN mxcdMute = { (LONG) state };
        MIXERCONTROLDETAILS mixerControlDetails;

        mixerControlDetails.cbStruct        = sizeof(MIXERCONTROLDETAILS);
        mixerControlDetails.dwControlID     = _controlID;
        mixerControlDetails.cChannels       = 1;
        mixerControlDetails.cMultipleItems  = 0;
        mixerControlDetails.cbDetails       = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
        mixerControlDetails.paDetails       = &mxcdMute;

        if (mixerSetControlDetails((HMIXEROBJ)_hMixer,
                                   &mixerControlDetails,
                                   MIXER_OBJECTF_HMIXER
                                   | MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
        {
            return FALSE;
        }
        return TRUE;
    }

private:
    /*************************************************************************/
    UINT _nNumMixers;
    HMIXER _hMixer;
    MIXERCAPS _mxcaps;
    DWORD _controlID;
};

