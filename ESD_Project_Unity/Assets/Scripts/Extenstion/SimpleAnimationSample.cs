using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SimpleAnimationSample : SingletonBehaviour<SimpleAnimationSample>
{
    #region Transform Animation

    public Coroutine AnimationLocalScale(Transform tran, float startScale, float endScale, float duration)
    {
        return StartCoroutine(LocalScale());
        IEnumerator LocalScale()
        {
            var currentTime = 0f;
            tran.localScale = new Vector3(startScale, startScale, startScale);

            while (currentTime <= duration)
            {
                if (tran == null) yield break;
                var time = currentTime / duration;
                var scale = Mathf.Lerp(startScale, endScale, time);
                tran.localScale = new Vector3(scale, scale, scale);

                currentTime += Time.deltaTime;
                yield return null;
            }

            tran.localScale = new Vector3(endScale, endScale, endScale);
        }
    }
    
    #endregion

    #region RectTransform Animation
    
    public Coroutine AnimationLocalMovement(RectTransform rect, Vector2 startPosition, Vector2 targetPosition, float duration)
    {
        return StartCoroutine(LocalMovement());
        IEnumerator LocalMovement()
        {
            var currentTime = 0f;
            rect.anchoredPosition = startPosition;
            
            while (currentTime <= duration)
            {
                if (rect == null) yield break;
                var time = currentTime / duration;
                rect.anchoredPosition = Vector2.Lerp(startPosition, targetPosition, time);
                currentTime += Time.deltaTime;
                yield return null;
            }
            rect.anchoredPosition = targetPosition;
        }
    }
    
    #endregion
    
    public Coroutine AnimationEndAction(System.Action callback, float duration)
    {
        return StartCoroutine(EndAction());
        IEnumerator EndAction()
        {
            var currentTime = 0f;
            while (currentTime <= duration)
            {
                currentTime += Time.deltaTime;
                yield return null;
            }
            callback?.Invoke();
        }
    }
}

