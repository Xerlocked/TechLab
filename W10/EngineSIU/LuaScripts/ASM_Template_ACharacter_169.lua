States = {
    { Name = "Entry", Sequence = "" },
    { Name = "Idle", Sequence = "Contents/Cloud_Idle.fbx::Armature|Cloud_Idle" },
    { Name = "Run", Sequence = "Contents/Cloud_Run.fbx::Armature|Cloud_Run" },
    { Name = "Jump", Sequence = "Contents/Cloud_Jump.fbx::Armature|Cloud_Jump" },
}

Transitions = {
    {
        From = "Entry",
        To = "Idle",
        Condition = function()
            return true
        end
    }, 
    {
        From = "Idle",
        To = "Run",
        Condition = function()
            return IsWalking()
        end
    },
    {
        From = "Run",
        To = "Idle",
        Condition = function()
            return not IsWalking()
        end
    },
    {
        From = "Idle",
        To = "Jump",
        Condition = function()
            return IsJumping()
        end
    },
    {
        From = "Jump",
        To = "Idle",
        Condition = function()
            return not IsJumping()
        end
    },
}