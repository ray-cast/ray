class FirstPersonCamera : IController
{
    float speed = 10.0;
    float gravity = 15;
    float maxVelocityChange = 1.0;
    float jumpHeight = 10;
    float lastX, lastY;
    int lockX = 0, lockY = 0;

    bool physical = true;

    GameObject self;

    FirstPersonCamera(GameObject& in ptr)
    {
        self = ptr;
    }

    void onActivate()
    {
        int sizeX = 1376;//display.getWindowWidth();
        int sizeY = 768;//display.getWindowHeight();

        lockX = sizeX >> 1;
        lockY = sizeY >> 1;

        input.setMousePos(lockX, lockY);
        input.lockCursor(true);
    }

    void onFrame()
    {
        float step = speed * timer.delta;

        if (input.getKey(keycode::lshift))
            step *= 3;

        if (!physical)
        {
            if (input.getKey(keycode::w))
                moveCamera(step);

            if (input.getKey(keycode::a))
                yawCamera(-step);

            if (input.getKey(keycode::s))
                moveCamera(-step);

            if (input.getKey(keycode::d))
                yawCamera(step);
        }
        else
        {
            float3 walkDirection(0, 0, 0);

            if (input.getKey(keycode::w))
            {
                float3 mov = self.lookat - self.translate;
                mov.normalize();
                walkDirection += mov;
            }
            if (input.getKey(keycode::s))
            {
                float3 mov = self.lookat - self.translate;
                mov.normalize();
                walkDirection -= mov;
            }
            if (input.getKey(keycode::a))
            {
                float3 mov = self.lookat - self.translate;
                mov = mov.cross(self.up);
                mov.normalize();
                walkDirection -= mov;
            }
            if (input.getKey(keycode::d))
            {
                float3 mov = self.lookat - self.translate;
                mov = mov.cross(self.up);
                mov.normalize();
                walkDirection += mov;
            }

            self.character.setWalkDirection(walkDirection * step);

            if (input.getKey(keycode::space))
            {
                if (self.character.canJumping())
                    self.character.jump(jumpHeight);
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
            if (input.getButton(button::mouse0))
            {
                input.setMousePos(lockX, lockY);
                input.lockCursor(true);
            }
        }
    }

    void yawCamera(float speed)
    {
        float3 mov = self.lookat - self.translate;
        mov = mov.cross(self.up);
        mov.normalize();
        mov *= speed;

        self.translate = self.translate + mov;
        self.lookat = self.lookat + mov;
    }

    void moveCamera(float speed)
    {
        float3 mov = self.lookat - self.translate;
        mov.normalize();
        mov *= speed;

        self.translate = self.translate + mov;
        self.lookat = self.lookat + mov;
    }

    void rotateCamera(float mouseX, float mouseY, float lastX, float lastY)
    {
        if (mouseX == lastX && mouseY == lastY)
            return;

        float angleY = (lastX - mouseX) / 100.0;
        float angleZ = (lastY - mouseY) / 100.0;

        float3 view = self.lookat - self.translate;
        view.normalize();

        float angle = view.dot(self.up) + angleZ;
        if (angle > -1.0 && angle < 1.0)
        {
            float3 axis = self.lookat - self.translate;
            axis = axis.cross(self.up);
            axis.normalize();

            rotateCamera(angleZ, axis);
        }

        rotateCamera(angleY, float3(0, 1, 0));
    }

    void rotateCamera(float angle, const float3 axis)
    {
        float3x3 rotate;
        rotate.makeRotate(angle, axis);

        float3 view = rotate * (self.lookat - self.translate);

        self.lookat = self.translate + view;
    }
}