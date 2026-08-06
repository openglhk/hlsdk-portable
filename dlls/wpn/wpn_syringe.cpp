void CSyringe::Precache( void )
{
    // 預載入第一人稱手持模型與地面/第三人稱世界模型
    PRECACHE_MODEL( "models/v_syringegun.mdl" );
    PRECACHE_MODEL( "models/w_syringegun.mdl" );

    // 向引擎註冊發射事件
    // 1 代表類型 (通常為 1 = 武器/玩家事件)
    // 註冊成功後會返回一個事件的唯一編號索引 (ID)
    m_usFireSyringe = PRECACHE_EVENT( 1, "events/syringe.sc" );
}
