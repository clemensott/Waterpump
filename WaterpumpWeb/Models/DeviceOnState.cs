﻿using System;

namespace WaterpumpWeb.Models
{
    public record DeviceOnState(bool IsForeverOn, DateTime OnUntil, bool? IsOn)
        : DeviceDesiredOnState(IsForeverOn, OnUntil);
}
