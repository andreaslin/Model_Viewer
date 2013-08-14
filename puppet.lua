-- My puppet

-- ROOT
rootnode = gr.node('root')

---------------------------------------------------------

-- MATERIALS
red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
black = gr.material({0.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
cool_cooper = gr.material({0.85, 0.53, 0.1}, {0.1, 0.1, 0.1}, 10)
skin = gr.material({0.99, 0.84, 0.61}, {0.1, 0.1, 0.1}, 10)

----------------------------------------------------------

--==================
--=== UPPER PART ===
--==================

-- NECK JOINT
neck_joint = gr.joint( 'neck_joint', {-15.0, 0.0, 15.0}, {-30.0, 0.0, 30.0} )

-- BODY
body = gr.sphere( 'body' )
body:scale( 0.7, 1.0, 0.5 )
body:set_material( white )

-- BODY JOINT
body_joint = gr.joint( 'body_joint', {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0} )

-- NECK
neck = gr.sphere( 'neck' )
neck:scale( 0.2, 0.3, 0.2 )
neck:set_material( white )

-- HEAD JOINT
head_joint = gr.joint( 'head_joint', {-15.0, 0.0, 15.0}, {-30.0, 0.0, 30.0} )

-- HEAD
head = gr.sphere( 'head' )
head:scale( 1.0, 1.0, 1.0 )
head:set_material( skin )

-- EYES
eye_left = gr.sphere( 'eye_left' )
eye_left:scale( 0.2, 0.2, 0.2 )
eye_left:set_material( white )

eye_right = gr.sphere( 'eye_right' )
eye_right:scale( 0.2, 0.2, 0.2 )
eye_right:set_material( white )

-- Pupil
pupil_left = gr.sphere( 'pupil_left' )
pupil_left:scale( 0.2, 0.18, 0.2 )
pupil_left:set_material( black )

pupil_right = gr.sphere( 'pupil_right' )
pupil_right:scale( 0.2, 0.18, 0.2 )
pupil_right:set_material( black )

--===============================
--=== Put all pieces together ===
--===============================
neck_joint:translate( 0.0, 1.0, 0.0 )
rootnode:add_child( neck_joint )

neck:translate( 0.0, 0.0, 0.0 )
neck_joint:add_child( neck )

head_joint:translate( 0.0, 0.0, 0.0 )
neck_joint:add_child( head_joint )

head:translate( 0.0, 1.0, 0.0 )
head_joint:add_child( head )

eye_left:translate( -1.9, 5.0, 4.0 )
head_joint:add_child( eye_left )

eye_right:translate( 1.9, 5.0, 4.0 )
head_joint:add_child( eye_right )

pupil_left:translate( -1.9, 5.55, 4.1 )
head_joint:add_child( pupil_left )

pupil_right:translate( 1.9, 5.55, 4.1 )
head_joint:add_child( pupil_right )

rootnode:add_child( body_joint )

body:translate( 0.0, 0.0, 0.0 )
body_joint:add_child( body )
----------------------------------------------------------

--===================
--=== MIDDLE PART ===
--===================

-- SHOULDER JOINT
shoulder_joint = gr.joint( 'shoulder_node', {-180.0, 0.0, 30.0}, {-30.0, 0.0, 30.0} )

-- SHOULDER
shoulder = gr.sphere( 'shoulder' )
shoulder:scale( 0.8, 0.2, 0.2 )
shoulder:set_material( white )

-- LEFT_ARM_JOINT
left_arm_joint = gr.joint( 'left_arm_joint', {-100.0, 0.0, 90.0}, {0.0, 0.0, 0.0} )

-- LEFT_UPPER_ARM
left_upper_arm = gr.sphere( 'left_upper_arm' )
left_upper_arm:scale( 0.1, 0.5, 0.1 )
left_upper_arm:set_material( white )

-- LEFT_ELBOW_JOINT
left_elbow = gr.joint( 'left_elbow', {-100.0, 0.0, 0.0}, {0.0, 0.0, 0.0} )

-- LEFT FOREARM
left_forearm = gr.sphere( 'left_forearm' )
left_forearm:scale( 0.1, 0.4, 0.1 )
left_forearm:set_material( white )

-- LEFT_WRIST
left_wrist = gr.joint( 'left_wrist', {-100.0, 0.0, 90.0}, {0.0, 0.0, 0.0} )

-- LEFT HAND
left_hand = gr.sphere( 'right_hand' )
left_hand:scale( 0.2, 0.2, 0.2 )
left_hand:set_material( black )

-- RIGHT_ARM_JOINT
right_arm_joint = gr.joint( 'right_arm_joint', {-100.0, 0.0, 90.0}, {0.0, 0.0, 0.0} )

-- RIGHT_UPPER_ARM
right_upper_arm = gr.sphere( 'right_upper_arm' )
right_upper_arm:scale( 0.1, 0.5, 0.1 )
right_upper_arm:set_material( white )

-- RIGHT_ELBOW_JOINT
right_elbow = gr.joint( 'right_elbow', {-100.0, 0.0, 0.0}, {0.0, 0.0, 0.0} )

-- RIGHT FOREARM
right_forearm = gr.sphere( 'right_forearm' )
right_forearm:scale( 0.1, 0.4, 0.1 )
right_forearm:set_material( white )

-- RIGHT WRIST
right_wrist = gr.joint( 'right_wrist', {-100.0, 0.0, 100.0}, {0.0, 0.0, 0.0} )

-- RIGHT HAND
right_hand = gr.sphere( 'right_hand' )
right_hand:scale( 0.2, 0.2, 0.2 )
right_hand:set_material( black )

--===============================
--=== Put all pieces together ===
--===============================
shoulder_joint:translate( 0.0, 0.7, 0.0 )
rootnode:add_child( shoulder_joint )

shoulder_joint:add_child( shoulder )

left_arm_joint:translate( -0.7, 0.0, 0.0 )
left_arm_joint:rotate( 'z', -30.0 )
shoulder_joint:add_child( left_arm_joint )

left_upper_arm:translate( 0.0, -0.8, 0.0 )
left_arm_joint:add_child( left_upper_arm )

left_elbow:translate( 0.0, -0.8, 0.0 )
left_arm_joint:add_child( left_elbow )

left_forearm:translate( 0.0, -0.8, 0.0 )
left_elbow:add_child( left_forearm )

left_wrist:translate( 0.0, -0.5, 0.0 )
left_elbow:add_child( left_wrist )

left_hand:translate( 0.0, 0.0, 0.0 )
left_wrist:add_child( left_hand )

right_arm_joint:translate( 0.7, 0.0, 0.0 )
right_arm_joint:rotate( 'z', 30.0 )
shoulder_joint:add_child( right_arm_joint )

right_upper_arm:translate( 0.0, -0.8, 0.0 )
right_arm_joint:add_child( right_upper_arm )

right_elbow:translate( 0.0, -0.8, 0.0 )
right_arm_joint:add_child( right_elbow )

right_forearm:translate( 0.0, -0.8, 0.0 )
right_elbow:add_child( right_forearm )

right_wrist:translate( 0.0, -0.5, 0.0 )
right_elbow:add_child( right_wrist )

right_hand:translate( 0.0, 0.0, 0.0 )
right_wrist:add_child( right_hand )

----------------------------------------------------------

--==================
--=== LOWER PART ===
--==================

-- HIPS JOINT
hips_joint = gr.joint( 'hips_joint', {-100.0, 0.0, 30.0}, {-20.0, 0.0, 20.0} )

-- HIPS
hips = gr.sphere( 'hips' )
hips:scale( 0.6, 0.2, 0.3 )
hips:set_material( white )

-- LEFT LEG JOINT
left_leg_joint = gr.joint( 'left_leg_joint', {-90.0, 0.0, 45.0}, {0.0, 0.0, 0.0} )

-- LEFT THIGH
left_thigh = gr.sphere( 'left_thigh' )
left_thigh:scale( 0.2, 0.5, 0.2 )
left_thigh:set_material( white )

-- LEFT KNEE
left_knee = gr.joint( 'left_knee', {0.0, 0.0, 100.0}, {0.0, 0.0, 0.0} )

-- LEFT CALF
left_calf = gr.sphere( 'left_calf' )
left_calf:scale( 0.15, 0.6, 0.15 )
left_calf:set_material( white )

-- LEFT ANKLE
left_ankle = gr.joint( 'left_ankle', {0.0, 0.0, 90.0}, {0.0, 0.0, 0.0} )

-- LEFT FOOT
left_foot = gr.sphere( 'left_foot' )
left_foot:scale( 0.2, 0.1, 0.2 )
left_foot:set_material( black )

-- RIGHT LEG JOINT
right_leg_joint = gr.joint( 'right_leg_joint', {-90.0, 0.0, 45.0}, {0.0, 0.0, 0.0} )

-- RIGHT THIGH
right_thigh = gr.sphere( 'right_thigh' )
right_thigh:scale( 0.2, 0.5, 0.2 )
right_thigh:set_material( white )

-- RIGHT KNEE
right_knee = gr.joint( 'right_knee', {0.0, 0.0, 100.0}, {0.0, 0.0,0.0} )

-- RIGHT CALF
right_calf = gr.sphere( 'right_calf' )
right_calf:scale( 0.15, 0.6, 0.15 )
right_calf:set_material( white )

-- RIGHT ANKLE
right_ankle = gr.joint( 'right_ankle', {0.0, 0.0, 90.0}, {0.0, 0.0, 0.0} )

-- RIGHT FOOT
right_foot = gr.sphere( 'right_foot' )
right_foot:scale( 0.2, 0.1, 0.2 )
right_foot:set_material( black )

--===============================
--=== Put all pieces together ===
--===============================
hips_joint:translate( 0.0, -0.8, 0.0 )
rootnode:add_child( hips_joint )

hips_joint:add_child( hips )

left_leg_joint:translate( -0.3, 0.0, 0.0 )
hips_joint:add_child( left_leg_joint )

left_thigh:translate( 0.0, -1.0, 0.0 )
left_leg_joint:add_child( left_thigh )

left_knee:translate( 0.0, -0.9, 0.0 )
left_leg_joint:add_child( left_knee )

left_calf:translate( 0.0, -0.8, 0.0 )
left_knee:add_child( left_calf )

left_ankle:translate( 0.0, -1.0, 0.0 )
left_knee:add_child( left_ankle )

left_foot:translate( 0.0, 0.0, 0.5 )
left_ankle:add_child( left_foot )

right_leg_joint:translate( 0.3, 0.0, 0.0 )
hips_joint:add_child( right_leg_joint )

right_thigh:translate( 0.0, -1.0, 0.0 )
right_leg_joint:add_child( right_thigh )

right_knee:translate( 0.0, -0.9, 0.0 )
right_leg_joint:add_child( right_knee )

right_calf:translate( 0.0, -0.8, 0.0 )
right_knee:add_child( right_calf )

right_ankle:translate( 0.0, -1.0, 0.0 )
right_knee:add_child( right_ankle )

right_foot:translate( 0.0, 0.0, 0.5 )
right_ankle:add_child( right_foot )


---------------------------------------------------

rootnode:translate( 0.0, 0.0, -10.0 )
rootnode:rotate( 'y', -30.0 )

return rootnode