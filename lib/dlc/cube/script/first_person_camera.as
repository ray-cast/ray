class FirstPersonCamera : IController
{
    float speed = 10.0;
    float gravity = 15;
    float maxVelocityChange = 1.0;
    float jumpHeight = 10;
    float lastX, lastY;
    int lockX = 0, lockY = 0;

    void onActivate()
    {
        lockX = display.getWindowWidth() >> 1;
        lockY = display.getWindowHeight() >> 1;
    }

    void onFrame()
    {
        float step = speed * timer.delta();

        if (input.getKey(keycode::lshift))
            step *= 3;

        if (!self.getPhysicsCharacter())
        {
            if (input.getKey(keycode::w))
                moveCamera(step);

            if (input.getKey(keycode::a))
                yawCamera(step);

            if (input.getKey(keycode::s))
                moveCamera(-step);

            if (input.getKey(keycode::d))
                yawCamera(-step);
        }
        else
        {
            float3 walkDirection(0, 0, 0);

            if (input.getKey(keycode::w))
            {
                float3 mov = self.getLookat() - self.getTranslate();
                mov.normalize();
                walkDirection += mov;
            }
            if (input.getKey(keycode::s))
            {
                float3 mov = self.getLookat() - self.getTranslate();
                mov.normalize();
                walkDirection -= mov;
            }
            if (input.getKey(keycode::a))
            {
                float3 mov = self.getLookat() - self.getTranslate();
                mov = mov.cross(self.getUp());
                mov.normalize();
                walkDirection += mov;
            }
            if (input.getKey(keycode::d))
            {
                float3 mov = self.getLookat() - self.getTranslate();
                mov = mov.cross(self.getUp());
                mov.normalize();
                walkDirection -= mov;
            }

            if (self.getPhysicsCharacter())
                self.getPhysicsCharacter().setWalkDirection(walkDirection * step);

            if (input.getKey(keycode::space))
            {
                if (self.getPhysicsCharacter().canJumping())
                    self.getPhysicsCharacter().jump(jumpHeight);
            }
        }

        if (input.getKey(keycode::esc))
        {
            input.lockCursor(false);
        }

        if (input.isLockedCursor())
        {
            rotateCamera(input.mousePositionX, input.mousePositionY, lockX, lockY);
        }
        else
        {
            if (input.getButton(button::mouse0) && !input.getKey(keycode::lctrl))
            {
                input.setMousePos(lockX, lockY);
                input.lockCursor(true);
            }
        }
    }

    void yawCamera(float speed)
    {
        float3 mov = self.getLookat() - self.getTranslate();
        mov = mov.cross(self.getUp());
        mov.normalize();
        mov *= speed;

        self.setTranslate(self.getTranslate() + mov);
        self.setLookat(self.getLookat() + mov);
    }

    void moveCamera(float speed)
    {
        float3 mov = self.getLookat() - self.getTranslate();
        mov.normalize();
        mov *= speed;

        self.setTranslate(self.getTranslate() + mov);
        self.setLookat(self.getLookat() + mov);
    }

    void rotateCamera(float mouseX, float mouseY, float lastX, float lastY)
    {
        if (mouseX == lastX && mouseY == lastY)
            return;

        float angleY = -(lastX - mouseX) / 100.0;
        float angleZ = (lastY - mouseY) / 100.0;

        float3 view = self.getLookat() - self.getTranslate();
        view.normalize();

        float angle = view.dot(self.getUp()) + angleZ;
        if (angle > -1.0 && angle < 1.0)
        {
            float3 axis = self.getLookat() - self.getTranslate();
            axis = axis.cross(self.getUp());
            axis.normalize();

            rotateCamera(angleZ, axis);
        }

        rotateCamera(angleY, float3(0, 1, 0));
    }

    void rotateCamera(float angle, const float3 axis)
    {
        float3x3 rotate;
        rotate.makeRotate(angle, axis);
        self.setLookat(self.getTranslate() + rotate * (self.getLookat() - self.getTranslate()));
    }
}