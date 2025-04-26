using UnityEngine;

public static class ExtensionUnityAnimation
{
    public static void AnimationLocalScale(this Transform tran, float startScale, float endScale, float duration) 
        => SimpleAnimationSample.Instance.AnimationLocalScale(tran, startScale, endScale, duration);
}