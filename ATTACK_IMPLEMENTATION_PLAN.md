# Attack Damage Logic Implementation Plan

## Current Architecture
- **Player**: Owns attack state (`isAttacking`, `attackSequence`, `facingRight`, `attackPendingClear`)
  - `getAttackInfo()` provides `AttackInfo` with hitbox, damage, direction
  - `finalizeAttackFrame()` clears attack state after animation
- **EnemyManager**: Resolves hits against all enemies
  - `resolvePlayerAttack(AttackInfo)` checks hitbox intersections
  - Prevents multiple hits per swing via `activeAttackSequence` + `enemiesHitThisAttack`
- **Enemy**: Takes damage
  - `takeDamage(amount)` reduces health, marks dead if health <= 0
- **Flow**: Level.update() → resolve collision → resolve attack → finalize attack

## Implementation Tasks

### 1. **Enhance Combat.h - Directional Hitbox Support**
   - Add `direction` to `AttackInfo` (enum: Left, Right, None)
   - Add helper struct `Hitbox` for reusable hitbox logic
   - Add enum `HitboxDirection` for clarity

### 2. **Enhance Player - Directional Hitbox Calculation**
   - Update `getAttackBounds()` to use `attackFacingRight` for direction
   - Add helper method `calculateDirectionalHitbox(sf.Vector2f center, HitboxDirection dir)`
   - Improve hitbox size/offset (configurable via constants)
   - Document attack animation frame timing

### 3. **Enhance Enemy - Hit Feedback**
   - Add `onHit(damage, knockback)` method for visual/sound feedback
   - Add optional knockback (vector based on hit direction)
   - Store recent hit time to avoid multiple hits per frame

### 4. **Enhance EnemyManager - Hit Resolution Logic**
   - Improve `resolvePlayerAttack()` documentation
   - Add debug visualization option (draw hitboxes)
   - Better handling of multiple enemies in hitbox
   - Consider damage falloff based on distance (future)

### 5. **Code Cleanup & Maintainability**
   - Add comprehensive comments to attack flow
   - Create helper namespace for hitbox calculations
   - Add constants for attack hitbox scaling
   - Document the once-per-swing mechanism

## Files to Modify
1. `src/Entities/Combat.h` - New structs/enums
2. `src/Entities/Player.h` - Add directional helpers
3. `src/Entities/Player.cpp` - Enhance hitbox logic
4. `src/Entities/Enemy.h` - Add feedback methods
5. `src/Entities/Enemy.cpp` - Implement damage feedback
6. `src/Entities/EnemyManager.cpp` - Improve resolution logic
7. `src/Entities/EnemyConfig.h` - Review for knockback support (if needed)

## Key Design Decisions
- Direction stored in AttackInfo for flexibility
- Once-per-swing ensured via sequence tracking (already implemented)
- Hitbox extends in attack direction only (no 360° hitbox)
- Enemy handles visual feedback independently
- All attack logic centralized in Level.update() order
