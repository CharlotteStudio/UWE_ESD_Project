using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using TMPro;

namespace UnityUISample
{
    [RequireComponent(typeof(Button))]
    public abstract class ButtonBase : MonoBehaviour, IPointerDownHandler, IPointerUpHandler
    {

        [SerializeField] protected Color fontColor = Color.black;
        [SerializeField, TextArea] protected string buttonText;
        
        [SerializeField] protected FontStyles fontStyles = FontStyles.Normal;

        [SerializeField] protected TMP_FontAsset textFont;
        [SerializeField] protected Sprite buttonBg;
        
        [SerializeField] protected float scaleSize = 0.9f;
        [SerializeField] protected float scaleDuration = 0.1f;
        
        [SerializeField] protected Button button;
        [SerializeField] protected Image imageBg;
        [SerializeField] protected TMP_Text TMPText;

        protected System.Action OnPointDownAction;
        protected System.Action OnPointUpAction;
        
        public bool interactable
        {
            get => button.interactable;
            set => button.interactable = value;
        }
        
        protected virtual void OnValidate()
        {
            if (button == null)
                button = GetComponent<Button>();
            if (TMPText == null)
                TMPText = GetComponentInChildren<TMP_Text>();
            if (buttonText != "")
                TMPText.text = buttonText;
            if (textFont != null)
                TMPText.font = textFont;
            if (buttonBg != null)
                imageBg.sprite = buttonBg;

            TMPText.color = fontColor;
            TMPText.fontStyle = fontStyles;
        }

        protected virtual void Start()
        {
            button.onClick.AddListener(BaseOnClickAction);
            button.onClick.AddListener(OnClickAction);
        }

        private void BaseOnClickAction()
        {
        }

        protected abstract void OnClickAction();

        public void OnPointerDown(PointerEventData eventData)
        {
            if (!button.interactable) return;
            OnPointDownAction?.Invoke();
            transform.AnimationLocalScale(1, scaleSize, scaleDuration);
        }

        public void OnPointerUp(PointerEventData eventData)
        { 
            if (!button.interactable) return;
            OnPointUpAction?.Invoke();
            transform.AnimationLocalScale(scaleSize, 1, scaleDuration);
        }
    }
}