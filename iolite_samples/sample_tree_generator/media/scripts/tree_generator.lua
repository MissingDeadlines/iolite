World.load()
Math.load()
Random.load()
Node.load()

-- Tree generator based on
-- https://nickmcd.me/2020/10/19/transport-oriented-growth-and-procedural-trees/

function BranchEmpty()
    return {
        parent = nil,
        child_a = nil,
        child_b = nil,
        depth = 0,
        dir = Vec3(0.0, 1.0, 0.0),
        length = 0.0,
        radius = 0.0,
        area = 0.1,
        radius_drawn = 0.0,
        length_drawn = 0.0
    }
end

function Branch(ratio, spread, split_size)
    local b = BranchEmpty()
    b.ratio = ratio
    b.spread = spread
    b.split_size = split_size

    return b
end

function BranchFromParent(parent)
    local b = BranchEmpty()
    b.ratio = parent.ratio
    b.spread = parent.spread
    b.split_size = parent.split_size
    b.depth = parent.depth + 1
    b.parent = parent

    return b
end

function IsLeaf(branch)
    return branch.child_a == nil
end

function IsRoot(branch)
    return branch.parent == nil
end

SplitDecay = 0.01
LocalDepth = 2
Directedness = 0.5

function LeafAverage(base_branch, branch)
    if IsLeaf(branch) then
        return Math.vec_scale(branch.length, branch.dir)
    end

    local a = Math.vec_scale(branch.length, branch.dir)
    local b = Math.vec_scale(base_branch.ratio, LeafAverage(base_branch, branch.child_a))
    local c = Math.vec_scale(1.0 - base_branch.ratio, LeafAverage(base_branch, branch.child_b))

    return Math.vec_add(a, Math.vec_add(b, c))
end

function LeafDensity(branch)
    local r = Math.vec_normalize(Vec3(Random.rand_float_min_max(-1.0, 1.0), Random.rand_float_min_max(-1.0, 1.0),
        Random.rand_float_min_max(-1.0, 1.0)))

    if not branch.parent then
        return r
    end

    local c = branch
    local search_depth = LocalDepth
    local rel = Vec3(0.0)
    while c and search_depth >= 0 do
        search_depth = search_depth - 1
        rel = Math.vec_add(rel, Math.vec_scale(c.length, c.dir))
        c = c.parent
    end

    local avg = LeafAverage(branch, branch)
    local dir_0 = Math.vec_scale(Directedness, Math.vec_normalize(Math.vec_sub(avg, rel)))
    local dir_1 = Math.vec_scale(1.0 - Directedness, r)

    return Math.vec_add(dir_0, dir_1)
end

function Split(branch)
    branch.child_a = BranchFromParent(branch)
    branch.child_b = BranchFromParent(branch)

    local d = LeafDensity(branch)
    local n = Math.vec_normalize(Math.vec_cross(branch.dir, d))
    local m = Math.vec_scale(-1.0, n)

    local flip = 1
    if Random.rand_float_min_max(0.0, 1.0) > 0.5 then
        flip = -1
    end

    local d0 = Math.vec_scale(flip * branch.spread, n)
    local d1 = Math.vec_scale(flip * branch.spread, m)

    branch.child_a.dir = Math.vec_normalize(Math.lerp(d0, branch.dir, branch.ratio))
    branch.child_b.dir = Math.vec_normalize(Math.lerp(d1, branch.dir, 1.0 - branch.ratio))
end

function Grow(branch, feed)
    branch.radius = Math.sqrt(branch.area / Math.pi)

    if IsLeaf(branch) then
        branch.length = branch.length + Math.pow(feed, 1.0 / 4.0)
        feed = feed - Math.pow(feed, 1.0 / 4.0) * branch.area
        branch.area = branch.area + feed / branch.length

        if branch.length > branch.split_size * Math.exp(-SplitDecay * branch.depth) then
            Split(branch)
        end
    else
        local pass = (branch.child_a.area + branch.child_b.area) /
                         (branch.child_a.area + branch.child_b.area + branch.area)
        branch.area = branch.area + pass * feed / branch.length
        feed = feed * (1.0 - pass)

        if feed < 0.00001 then
            return
        end

        Grow(branch.child_a, feed * branch.ratio)
        Grow(branch.child_b, feed * (1.0 - branch.ratio))
    end
end

function UpdateVoxels(pos, branch)
    local next_pos = Math.vec_add(pos, Math.vec_scale(branch.length, branch.dir))

    local radius = Math.clamp(branch.area, 0.2, 0.75)
    local threshold = 0.25

    if Math.abs(branch.radius_drawn - radius) > threshold or Math.abs(branch.length_drawn - branch.length) > threshold then
        local steps = branch.length / radius

        for i = 1, steps + 1 do
            local p = Math.vec_add(pos, Math.vec_scale((i - 1) * radius, branch.dir))
            World.draw_sphere(p, radius, 1, "tree_chunk")
        end

        local center = Math.vec_scale(0.5, Math.vec_add(pos, next_pos))

        local leaf_depth = 6
        local flower_depth = 8

        if branch.depth >= flower_depth then
            local radius = Random.rand_float_min_max(0.5, 0.9)
            World.draw_sphere(center, radius, 4, "tree_chunk")
            World.draw_sphere_noise(center, radius * 2, 128, 4, "tree_chunk")
        end
        if branch.depth >= leaf_depth then
            local radius = Random.rand_float_min_max(0.5, 0.75)
            World.draw_sphere(center, radius, 3, "tree_chunk")
            World.draw_sphere_noise(center, radius * 2, 512, 3, "tree_chunk")
        end

        branch.radius_drawn = radius
        branch.length_drawn = branch.length
    end

    if not IsLeaf(branch) then
        UpdateVoxels(next_pos, branch.child_a)
        UpdateVoxels(next_pos, branch.child_b)
    end
end

---Called once when the script component becomes active.
---@param entity Ref The ref of the entity the script component is attached to.
function OnActivate(entity)
    TreeRoot = Branch(0.6, 0.45, 2.5)
end

--- Called every frame.
---@param entity Ref The ref of the entity the script component is attached to.
---@param delta_t number The time (in seconds) passed since the last call to this function.
function Tick(entity, delta_t)
end

--- Called at the frequency of the update interval specified in the component.
---@param entity Ref The ref of the entity the script component is attached to.
---@param delta_t number The time (in seconds) passed since the last call to this function.
function Update(entity, delta_t)
    Grow(TreeRoot, 1.0)
    local node = Node.get_component_for_entity(entity)
    UpdateVoxels(Node.get_world_position(node), TreeRoot)
end

--- Called once when the script component becomes inactive.
---@param entity Ref The ref of the entity the script component is attached to.
function OnDeactivate(entity)
end

--- Event callback
---@param entity Ref The ref of the entity the script component is attached to.
---@param events table List of all events.
function OnEvent(entity, events)
end
