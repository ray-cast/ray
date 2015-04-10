class Billboard : IController
{
    void onFrame()
    {
        Vector3 view = transform.lookat - transform.translate;
        view.normalize();

        float dot = view.dot(transform.up);

        float angle = acos(dot);

        if (translate.x > 0)
            angle = 3.14 - angle;
        else
            angle += 3.14;

        transform->setRotateY(angle);
    }
}