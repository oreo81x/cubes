bool inline select_block()
{
    const glm::ivec3 block_pos  = glm::ivec3(m_cubes_center[m_controlled_cube]);
    const glm::ivec3 cursor_pos = glm::ivec3(m_cursor_cube_center);

    if (m_controlled_cube > -1 && cursor_pos == block_pos)
    {
        return true;
    }

    m_controlled_cube = -1;

    for (uint32_t id = 0u; id < m_cubes_center.size(); id++)
        if (glm::ivec3(m_cubes_center[id]) == cursor_pos)
        {
            return (m_controlled_cube = id) > -1;
        }

    return false;
}

void inline add_block()
{
    if (select_block())
    {
        return static_cast<void>(m_cubes_texture[m_controlled_cube] = m_next_texture);
    }

    m_cubes_center.emplace_back(static_cast<glm::ivec3>(m_cursor_cube_center));
    m_cubes_texture.emplace_back(m_next_texture);

    m_controlled_cube = m_cubes_center.size() - 1ull;
}

void inline remove_block()
{
    if (select_block())
    {
        m_cubes_center.erase(m_cubes_center.begin()    + m_controlled_cube);
        m_cubes_texture.erase(m_cubes_texture.begin()  + m_controlled_cube);
    }

    m_controlled_cube = -1;
}

void inline translate_block()
{
    m_cubes_center[m_controlled_cube] = static_cast<glm::ivec3>(m_cursor_cube_center);
}

void inline change_block_texture_set(uint32_t a_texture_set)
{
    m_next_texture = a_texture_set;

    if (select_block())
    {
        m_cubes_texture[m_controlled_cube] = m_next_texture;
    }
}