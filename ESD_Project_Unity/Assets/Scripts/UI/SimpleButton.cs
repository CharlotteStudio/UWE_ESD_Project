using UnityEngine;
using UnityEngine.Events;
using UnityEngine.UI;

namespace UnityUISample
{
    [RequireComponent(typeof(Button))]
    public class SimpleButton : ButtonBase
    {
        public UnityEvent OnClickEvent;

        public void AddOnClickAction(System.Action action) => OnClickEvent.AddListener(() => action?.Invoke());
        public void RemoveAllAction() => OnClickEvent = new UnityEvent();
        
        protected override void OnClickAction() =>
            OnClickEvent?.Invoke();
    }
}
