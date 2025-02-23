import os
from dotenv import load_dotenv
from langchain_groq import ChatGroq
from langchain_huggingface import HuggingFaceEmbeddings
from langchain_community.vectorstores import InMemoryVectorStore
from langchain_text_splitters import RecursiveCharacterTextSplitter
from langchain_community.document_loaders import TextLoader
from langchain_core.prompts import PromptTemplate
from langchain_core.runnables import RunnableLambda
from langchain_core.messages import HumanMessage
from typing import TypedDict

# Prevent tokenizer warnings/errors
os.environ["TOKENIZERS_PARALLELISM"] = "false"

# Load environment variables
load_dotenv()
GROQ_API_KEY = os.getenv("GROQ_API_KEY")

# Initialize the embeddings model
embeddings_model = HuggingFaceEmbeddings(model_name="intfloat/multilingual-e5-large-instruct")

# Setup text splitter and document loader
text_splitter = RecursiveCharacterTextSplitter(chunk_size=500, chunk_overlap=100)
loader = TextLoader("yoda.txt", encoding="utf-8")

# Load and split the document into chunks
document = loader.load()
chunks = text_splitter.split_documents(document)

# Create an in-memory vector store from the document chunks
vector_store = InMemoryVectorStore.from_documents(chunks, embeddings_model)
retriever = vector_store.as_retriever()

print("1 ")

# Initialize the language model
llm = ChatGroq(
    model_name="llama-3.3-70b-versatile",
    temperature=0.5,
    api_key=GROQ_API_KEY
)

print("2 ")

class State(TypedDict):
    messages: list
    context: str

def rag_search(state: State) -> State:
    print("3 ")
    """Retrieve relevant context from the vector store"""
    if not state["messages"]:
        return {"context": ""}
    
    last_message = state["messages"][-1]
    
    docs = retriever.invoke(last_message.content)
    context = "\n".join(doc.page_content for doc in docs)
    return {"context": context}

def answer(state: State) -> State:
    print("4 ")

    """Generate a response using the LLM and context"""

    prompt = PromptTemplate.from_template("""You are Yoda from Star Wars, running your restaurant 'Yoda's Galactic Feast'.
Answer the question using the provided context. Speak in Yoda's tone of voice.
Answer in a conversational tone in plain text without quotes.

Context: {context}

Question: {question}

Answer:
""")
    
    last_message = state["messages"][-1]
    
    # Create the chain
    chain = prompt | llm
    
    # Generate response
    response = chain.invoke({
        "context": state["context"],
        "question": last_message.content
    })
    
    return {"messages": [response]}

print("5 ")

# Create a sequential execution pipeline instead of RunnableGraph
def process_message(state: State) -> State:
    state.update(rag_search(state))  # Add context
    state.update(answer(state))  # Generate answer
    return state

print("6 ")

def chat():
    """Run the interactive chat loop"""
    print("\nWelcome to Yoda's Galactic Feast Chat! Type 'quit' to exit.\n")
    
    state = {
        "messages": [],
        "context": ""
    }
    
    while True:
        user_input = input("\nUser: ")
        if user_input.lower() in ["quit", "exit", "q"]:
            print("\nMay the Force be with you!")
            break
        
        state["messages"].append(HumanMessage(content=user_input))
        result = process_message(state)
        
        messages = result.get("messages", [])
        if messages:
            last_message = messages[-1]
            print("\nYoda:", last_message.content)

if __name__ == "__main__":
    chat()
