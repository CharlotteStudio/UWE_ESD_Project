using UnityEngine;

public static class ExtensionUnityAnimation
{
    public static void AnimationLocalScale(this Transform tran, float startScale, float endScale, float duration) 
        => SimpleAnimationSample.Instance.AnimationLocalScale(tran, startScale, endScale, duration);
    
    public static void AnimationLocalMovement(this RectTransform rect, Vector2 startPosition, Vector2 targetPosition, float duration) 
        => SimpleAnimationSample.Instance.AnimationLocalMovement(rect, startPosition, targetPosition, duration);
    
    public static void AnimationFadeIn(this CanvasGroup canvasGroup, float duration) 
        => SimpleAnimationSample.Instance.AnimationFade(canvasGroup, 0, 1, duration);
    
    public static void AnimationFadeOut(this CanvasGroup canvasGroup, float duration) 
        => SimpleAnimationSample.Instance.AnimationFade(canvasGroup, 1, 0, duration);
}