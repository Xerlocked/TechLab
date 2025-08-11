States = {
    { Name = "Entry", Sequence = "" },
    { Name = "Idle", Sequence = "Contents/Loco/idle.fbx::mixamo.com" },
    { Name = "Walk", Sequence = "Contents/Loco/walking.fbx::mixamo.com" },
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
        To = "Walk",
        Condition = function()
            return IsWalking()
        end
    },
    {
        From = "Walk",
        To = "Idle",
        Condition = function()
            return not IsWalking()
        end
    },
}