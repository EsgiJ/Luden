
#include <rttr/registration>
using namespace rttr;
registration::class_<IComponent>("IComponent")
    .property("has", &IComponent::has)
;

registration::class_<CDamage>("CDamage")
    .constructor<>()
    .constructor<int d>()
    .property("damage", &CDamage::damage)
;

registration::class_<CDraggable>("CDraggable")
    .constructor<>()
    .property("dragging", &CDraggable::dragging)
;

registration::class_<CFollowPlayer>("CFollowPlayer")
    .constructor<>()
    .constructor<Math::Vec2 p, float s>()
    .property("home", &CFollowPlayer::home)
    .property("speed", &CFollowPlayer::speed)
;

registration::class_<CGravity>("CGravity")
    .constructor<>()
    .constructor<float g>()
    .property("gravity", &CGravity::gravity)
;

registration::class_<CHealth>("CHealth")
    .constructor<>()
    .constructor<int m, int c>()
    .property("max", &CHealth::max)
    .property("current", &CHealth::current)
;

registration::class_<CInput>("CInput")
    .constructor<>()
    .property("up", &CInput::up)
    .property("down", &CInput::down)
    .property("left", &CInput::left)
    .property("right", &CInput::right)
    .property("attack", &CInput::attack)
    .property("canAttack", &CInput::canAttack)
;

registration::class_<CBoundingBox>("CBoundingBox")
    .constructor<>()
    .constructor<const Math::Vec2& c, const Math::Vec2& s, bool m, bool v>()
    .property("size", &CBoundingBox::size)
    .property("halfSize", &CBoundingBox::halfSize)
    .property("center", &CBoundingBox::center)
    .property("prevCenter", &CBoundingBox::prevCenter)
    .property("blockMove", &CBoundingBox::blockMove)
    .property("blockVision", &CBoundingBox::blockVision)
;

registration::class_<CAnimation>("CAnimation")
    .constructor<>()
    .constructor<Graphics::Animation a, bool r>()
    .property("animation", &CAnimation::animation)
    .property("repeat", &CAnimation::repeat)
;

registration::class_<CInvincibility>("CInvincibility")
    .constructor<>()
    .constructor<int f>()
    .property("iframes", &CInvincibility::iframes)
;

registration::class_<CLifespan>("CLifespan")
    .constructor<>()
    .constructor<int duration, int frame>()
    .property("lifespan", &CLifespan::lifespan)
    .property("frameCreated", &CLifespan::frameCreated)
;

registration::class_<CPatrol>("CPatrol")
    .constructor<>()
    .constructor<std::vector<Math::Vec2>& pos, float s>()
    .property("positions", &CPatrol::positions)
    .property("currentPosition", &CPatrol::currentPosition)
;

registration::class_<CState>("CState")
    .constructor<>()
    .constructor<std::string s>()
    .property("state", &CState::state)
    .property("previousState", &CState::previousState)
;

registration::class_<CTransform>("CTransform")
    .constructor<>()
    .constructor<const Math::Vec2& p>()
    .constructor<const Math::Vec2& p, const Math::Vec2& speed, const Math::Vec2& s, float a>()
    .property("pos", &CTransform::pos)
    .property("prevPos", &CTransform::prevPos)
    .property("velocity", &CTransform::velocity)
    .property("scale", &CTransform::scale)
    .property("facing", &CTransform::facing)
    .property("angle", &CTransform::angle)
;

